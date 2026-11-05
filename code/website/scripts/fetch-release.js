#!/usr/bin/env node
// Writes data/latest.json from the newest published GitHub Release.
// CI runs this right before the Docker image is built, so every image ships
// with the current download links baked in (no browser-side rate limits).
//
//   GITHUB_REPOSITORY=owner/repo GITHUB_TOKEN=... node scripts/fetch-release.js
//
// A missing release is not an error: the file is written with "tag": null and
// the download page falls back to asking the GitHub API itself.
const fs = require("node:fs");
const path = require("node:path");
const { buildLatest } = require("../js/release-data.js");

async function run({ repo, token, fetchFn = fetch, outPath, fsImpl = fs, now = () => new Date() }) {
  if (!repo) throw new Error("GITHUB_REPOSITORY is not set (expected owner/name)");
  const headers = { Accept: "application/vnd.github+json", "User-Agent": "quadm-website-build" };
  if (token) headers.Authorization = `Bearer ${token}`;

  let latest = null;
  let note = "";
  try {
    const res = await fetchFn(`https://api.github.com/repos/${repo}/releases/latest`, { headers });
    if (res.status === 404) note = "no published release yet";
    else if (!res.ok) note = `GitHub answered ${res.status}`;
    else latest = buildLatest(await res.json(), repo);
  } catch (err) {
    note = `lookup failed: ${err.message}`;
  }

  const out = latest || { repo, tag: null, version: null, assets: {} };
  out.generatedAt = now().toISOString();
  fsImpl.mkdirSync(path.dirname(outPath), { recursive: true });
  fsImpl.writeFileSync(outPath, JSON.stringify(out, null, 2) + "\n");
  return { latest, note, outPath };
}

module.exports = { run };

if (require.main === module) {
  run({
    repo: process.env.GITHUB_REPOSITORY,
    token: process.env.GITHUB_TOKEN,
    outPath: process.env.OUT || path.join(__dirname, "..", "data", "latest.json")
  }).then(({ latest, note, outPath }) => {
    if (latest) console.log(`Wrote ${outPath}: ${latest.tag}`);
    else console.log(`::warning::${note}; wrote ${outPath} without a release (page will ask GitHub directly)`);
  }).catch(err => { console.error(err.message); process.exit(1); });
}
