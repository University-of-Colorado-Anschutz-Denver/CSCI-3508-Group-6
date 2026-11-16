const test = require("node:test");
const assert = require("node:assert");
const fs = require("node:fs");
const path = require("node:path");
const { validate, buildCommit, submit, randomId } = require("../js/feedback-service.js");

const good = { type: "bug", message: "  Crash on open  ", email: "a@b.co", version: "0.1.0" };

test("validate accepts a good record and trims it", () => {
  const v = validate(good);
  assert.ok(v.ok);
  assert.deepStrictEqual(v.clean, { type: "bug", message: "Crash on open", email: "a@b.co", appVersion: "0.1.0" });
});

test("validate: email and version are optional", () => {
  const v = validate({ type: "other", message: "hi", email: "", version: "" });
  assert.ok(v.ok);
  assert.deepStrictEqual(Object.keys(v.clean), ["type", "message"]);
});

test("validate rejects bad type, empty or long message, bad email, bad version", () => {
  assert.ok(validate({ ...good, type: "rant" }).errors.type);
  assert.ok(validate({ ...good, message: "   " }).errors.message);
  assert.ok(validate({ ...good, message: "x".repeat(2001) }).errors.message);
  assert.ok(validate({ ...good, message: "x".repeat(2000) }).ok);
  assert.ok(validate({ ...good, email: "nope" }).errors.email);
  assert.ok(validate({ ...good, version: "1 2 3" }).errors.version);
});

test("buildCommit uses a server timestamp, status new, and create-only", () => {
  const c = buildCommit(validate(good).clean, "proj", "abc");
  const w = c.writes[0];
  assert.strictEqual(w.update.name, "projects/proj/databases/(default)/documents/feedback/abc");
  assert.deepStrictEqual(w.updateTransforms, [{ fieldPath: "createdAt", setToServerValue: "REQUEST_TIME" }]);
  assert.deepStrictEqual(w.currentDocument, { exists: false });
  assert.strictEqual(w.update.fields.status.stringValue, "new");
  assert.ok(!("createdAt" in w.update.fields), "browser never sends createdAt");
  assert.deepStrictEqual(w.updateMask.fieldPaths.sort(), Object.keys(w.update.fields).sort());
});

test("submit posts to the Firestore commit URL and reports success", async () => {
  let call;
  const fetchFn = async (url, opts) => { call = { url, opts }; return { ok: true, status: 200 }; };
  const r = await submit(good, { projectId: "proj", apiKey: "K Y" }, fetchFn, () => "id1");
  assert.deepStrictEqual(r, { ok: true });
  assert.strictEqual(call.url, "https://firestore.googleapis.com/v1/projects/proj/databases/(default)/documents:commit?key=K%20Y");
  assert.strictEqual(call.opts.method, "POST");
  assert.match(JSON.parse(call.opts.body).writes[0].update.name, /feedback\/id1$/);
});

test("submit reports invalid, not-configured, http and network failures without calling fetch when it should not", async () => {
  let n = 0;
  const ok = async () => { n++; return { ok: true }; };
  assert.strictEqual((await submit({ type: "bug", message: "" }, { projectId: "p", apiKey: "k" }, ok)).reason, "invalid");
  assert.strictEqual((await submit(good, { projectId: "", apiKey: "" }, ok)).reason, "not-configured");
  assert.strictEqual(n, 0);
  assert.strictEqual((await submit(good, { projectId: "p", apiKey: "k" }, async () => ({ ok: false, status: 403 }))).reason, "http-403");
  assert.strictEqual((await submit(good, { projectId: "p", apiKey: "k" }, async () => { throw new Error("x"); })).reason, "network");
});

test("randomId is 20 alphanumeric characters", () => {
  assert.match(randomId(), /^[A-Za-z0-9]{20}$/);
});

test("firestore.rules only allows create of the schema fields and denies everything else", () => {
  const rules = fs.readFileSync(path.join(__dirname, "..", "firebase", "firestore.rules"), "utf8");
  assert.match(rules, /allow create: if valid\(request\.resource\.data\)/);
  assert.match(rules, /allow read, update, delete: if false/);
  assert.match(rules, /d\.createdAt == request\.time/);
  assert.match(rules, /d\.status == 'new'/);
  assert.match(rules, /d\.message\.size\(\) <= 2000/);
  for (const t of ["bug", "feature", "question", "other"]) assert.ok(rules.includes(`'${t}'`));
  assert.ok(!/allow (read|write)[^;]*: if true/.test(rules), "no open access");
});
