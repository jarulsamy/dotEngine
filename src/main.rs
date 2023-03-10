use chrono;
use clap::Parser;
use home;
use ini::Ini;
use octocrab::Octocrab;
use serde::{Deserialize, Serialize};
use serde_json::Value;
use std::io;
use std::{
    error::Error,
    fs::{self, File},
    io::Write,
    path::PathBuf,
};

#[derive(Debug)]
struct UserConfig {
    github_username: String,
    github_token: String,
    cache_path: PathBuf,
}

impl UserConfig {
    pub fn new(home_dir: &PathBuf) -> Result<Self, Box<dyn Error>> {
        let mut config_path = PathBuf::new();
        config_path.push(home_dir.clone());
        config_path.push(".gitconfig");

        let mut cache_path = PathBuf::new();
        cache_path.push(home_dir);
        cache_path.push(".dot-engine.tmp");

        let ini_conf = Ini::load_from_file(config_path)?;
        let section = ini_conf
            .section(Some("dot-engine"))
            .ok_or::<Box<dyn Error>>("Config missing required section 'dot-engine'".into())?;
        let github_username = section
            .get("github-username")
            .ok_or::<Box<dyn Error>>("Config missing required field 'github-username'".into())?
            .to_string();
        let github_token = section
            .get("github-token")
            .ok_or::<Box<dyn Error>>("Config missing required field 'github-token'".into())?
            .to_string();

        let config = UserConfig {
            github_username,
            github_token,
            cache_path,
        };

        Ok(config)
    }
}

#[derive(Debug, Serialize, Deserialize)]
struct Repository {
    name: String,
    description: Option<String>,
    owner: String,
    ssh_url: String,
}

impl Repository {
    pub fn new(v: &Value) -> Option<Self> {
        let name = &v["name"].as_str()?;
        let description = match &v["description"] {
            Value::String(d) => Some(d.to_string()),
            _ => None,
        };
        let owner = &v["owner"]["login"].as_str()?;
        let ssh_url = &v["sshUrl"].as_str()?;

        Some(Self {
            name: name.to_string(),
            description,
            owner: owner.to_string(),
            ssh_url: ssh_url.to_string(),
        })
    }
}

#[derive(Debug, Serialize, Deserialize)]
struct RepositoryCache {
    last_fetch_time: chrono::NaiveTime,
    repos: Vec<Repository>,
}

impl RepositoryCache {
    pub fn new(repos: Vec<Repository>) -> Self {
        let last_fetch_time = chrono::offset::Utc::now().time();
        Self {
            last_fetch_time,
            repos,
        }
    }

    pub fn from_disk(path: &PathBuf) -> Option<Self> {
        let now = chrono::Utc::now().time();
        let data = fs::read_to_string(path).ok()?;
        let res: Self = serde_json::from_str(&data).ok()?;

        let delta_min = (now - res.last_fetch_time).num_minutes();
        if delta_min > 5 {
            // Stale cache
            return None;
        }

        Some(res)
    }

    pub fn to_disk(&self, path: &PathBuf) -> Result<(), io::Error> {
        let mut file = File::create(&path)?;
        let contents = serde_json::to_string(&self).unwrap_or_default();
        file.write_all(contents.as_bytes())?;

        Ok(())
    }
}

#[derive(Debug, Parser)]
#[command(author, version, about)]
struct Cli {
    /// Ignore the cache and force an API call to GitHub.
    #[arg(short, long)]
    force: bool,

    /// Output to FILE instead of stdout
    #[arg(short, long, value_name = "FILE")]
    output: Option<PathBuf>,

    /// Output in a tabular format
    #[arg(short, long, group = "output_type")]
    table: bool,

    /// Output ZSH autocompletion information
    #[arg(short, long, group = "output_type")]
    zsh_autocompletion: bool,
}

impl Default for Cli {
    fn default() -> Self {
        Self {
            force: false,
            output: None,
            table: false,
            zsh_autocompletion: false,
        }
    }
}

async fn get_repos(config: &UserConfig) -> octocrab::Result<Vec<Repository>> {
    macro_rules! QUERY {
        () => {
            r#"{{
                 viewer {{
                 repositories(
                  first: 100
                   affiliations: [OWNER, ORGANIZATION_MEMBER, COLLABORATOR]
                   ownerAffiliations: [OWNER, ORGANIZATION_MEMBER, COLLABORATOR]
                   after: {after}
                 ) {{
                     totalCount
                     pageInfo {{
                       endCursor
                       hasNextPage
                     }}
                     nodes {{
                       name
                       owner {{
                           login
                       }}
                       sshUrl
                       description
                     }}
                   }}
                 }}
               }}"#
        };
    }

    let mut repos: Vec<Repository> = Vec::with_capacity(256);

    let octocrab = Octocrab::builder()
        .personal_token(config.github_token.clone())
        .build()?;

    // Grab a set of repos, continue querying until all repos are retrieved.
    let mut after: String = "null".to_string();

    loop {
        let query = format!(QUERY!(), after = after);
        let resp: serde_json::Value = octocrab.graphql(&query).await?;
        let data = &resp["data"]["viewer"]["repositories"];

        let nodes = &data["nodes"];
        match nodes {
            Value::Array(values) => {
                for v in values {
                    if let Some(repo) = Repository::new(v) {
                        repos.push(repo);
                    }
                }
            }
            _ => break, // Encountered some erroneous data...
        };

        let has_next_page = &data["pageInfo"]["hasNextPage"];
        if !has_next_page.as_bool().unwrap_or(false) {
            break;
        } else {
            after = format!(
                "\"{}\"",
                data["pageInfo"]["endCursor"].as_str().unwrap().to_string()
            );
        }
    }

    Ok(repos)
}

fn table(out: &mut (impl Write + ?Sized), data: &Vec<Repository>) -> Result<(), io::Error> {
    for r in data {
        let descrip = match &r.description {
            Some(x) => x,
            None => "<None>",
        };
        writeln!(
            out,
            "{:<32}\t{:<256}\t{:<32}\t{:<32}",
            r.name, descrip, r.owner, r.ssh_url
        )?;
    }

    Ok(())
}

fn zsh_autocompletion(
    out: &mut (impl Write + ?Sized),
    data: &Vec<Repository>,
) -> Result<(), io::Error> {
    // write!(out, "\"(")?;
    for i in &data[..1] {
        let descrip = match &i.description {
            Some(x) => "foo",
            None => "none",
        };
        if i.name.contains(':') || descrip.contains(':') {
            continue;
        }
        write!(out, "'{}'", i.name)?;
    }
    // writeln!(out, ")\"")?;

    Ok(())
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    let args = Cli::parse();

    // Find the gitconfig file
    let user_home = home::home_dir().expect("Unable to determine user home directory!");
    let config = UserConfig::new(&user_home).unwrap();

    let cache;

    let repos;
    if args.force {
        let r = get_repos(&config).await.unwrap();
        cache = RepositoryCache::new(r);
        // It's okay if we can't cache the result.
        cache.to_disk(&config.cache_path).ok();
        repos = cache.repos;
    } else {
        repos = match RepositoryCache::from_disk(&config.cache_path) {
            Some(r) => r.repos,
            None => {
                let r = get_repos(&config).await.unwrap();
                cache = RepositoryCache::new(r);
                // It's okay if we can't cache the result.
                cache.to_disk(&config.cache_path).ok();
                cache.repos
            }
        };
    }

    if args.table {
        table(&mut io::stdout(), &repos).unwrap();
    } else if args.zsh_autocompletion {
        zsh_autocompletion(&mut io::stdout(), &repos).unwrap();
    }

    Ok(())
}
