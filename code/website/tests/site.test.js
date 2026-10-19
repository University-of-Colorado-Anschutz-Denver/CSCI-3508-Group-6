// Static website validation suite. Run with: npm test.
// Import the Node.js testing and assertion utilities required for the checks.
const test = require("node:test");
const assert = require("node:assert");
const fs = require("node:fs");
const path = require("node:path");
const vm = require("node:vm");

// Set the website root and the list of pages under validation.
const ROOT = path.join(__dirname, "..");
const PAGES = ["index.html", "download.html", "guide.html", "feedback.html", "about.html"];

// Read a file from the website root as UTF-8 content.
const read = f => fs.readFileSync(path.join(ROOT, f), "utf8");

// Validate the structural integrity of each page in the site.
for (const page of PAGES) {
  // Verify that the page exists and includes the required semantic HTML elements.
  test(`${page}: basic structure`, () => {
    assert.ok(fs.existsSync(path.join(ROOT, page)), "file exists");
    const html = read(page);
    assert.match(html, /^<!doctype html>/i, "starts with doctype");
    assert.match(html, /<html[^>]+lang="/, "has lang attribute");
    assert.match(html, /<meta name="viewport"/, "has viewport meta");
    assert.match(html, /<title>[^<]+<\/title>/, "has non-empty title");
    assert.strictEqual((html.match(/<h1[\s>]/g) || []).length, 1, "exactly one h1");
    assert.match(html, /<main>/, "has main landmark");
  });

  // Ensure all local links and asset references resolve to valid files.
  test(`${page}: local links and assets resolve`, () => {
    const html = read(page);
    const refs = [...html.matchAll(/(?:href|src)="([^"]+)"/g)].map(m => m[1]);
    for (const ref of refs) {
      if (/^(https?:|mailto:|#)/.test(ref)) continue;
      const [file, hash] = ref.split("#");
      assert.ok(fs.existsSync(path.join(ROOT, file)), `${page} references missing file: ${ref}`);
      if (hash && file.endsWith(".html")) {
        assert.ok(new RegExp(`id="${hash}"`).test(read(file)), `${page} links to missing anchor: ${ref}`);
      }
    }
  });

  // Check that all image elements provide accessible alt text.
  test(`${page}: images have alt text`, () => {
    for (const tag of read(page).match(/<img\b[^>]*>/g) || []) {
      assert.match(tag, /\balt="[^"]*"/, `missing alt: ${tag}`);
    }
  });
}

// Verify that each page includes the same primary navigation entries.
test("every page appears in the main navigation", () => {
  for (const page of PAGES) {
    const nav = read(page).match(/<nav aria-label="Main">[\s\S]*?<\/nav>/)[0];
    for (const target of PAGES) assert.ok(nav.includes(`href="${target}"`), `${page} nav missing ${target}`);
  }
});

// Confirm that the JavaScript bundle parses successfully without syntax errors.
test("js/main.js parses without syntax errors", () => {
  assert.doesNotThrow(() => new vm.Script(read("js/main.js"), { filename: "main.js" }));
});

// Validate the required fields in the feedback form before submission.
test("feedback form has the required fields", () => {
  const html = read("feedback.html");
  for (const name of ["type", "email", "message"]) {
    assert.match(html, new RegExp(`name="${name}"`), `missing field ${name}`);
  }
  assert.match(html, /<textarea[^>]*required/, "message is required");
});

// Confirm that the download page exposes the elements required by the client-side script.
test("download page has hooks used by main.js", () => {
  const html = read("download.html");
  for (const id of ["version", "dl-win", "dl-linux"]) assert.match(html, new RegExp(`id="${id}"`));
});

// Optional release gate to detect unfinished placeholders in production builds.
test("no unfinished placeholders in the Release build", { skip: !process.env.STRICT_PLACEHOLDERS }, () => {
  for (const f of [...PAGES, "js/main.js"]) {
    assert.ok(!read(f).includes("YOUR-USERNAME"), `${f} still contains YOUR-USERNAME`);
  }
});
