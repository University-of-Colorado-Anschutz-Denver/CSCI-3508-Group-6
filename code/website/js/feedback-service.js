// Validates the feedback form and writes it to Cloud Firestore through the
// public REST API. Shared by the browser and Node (tests). The same rules are
// enforced a second time, server side, by firebase/firestore.rules.
(function (root, factory) {
  if (typeof module === "object" && module.exports) module.exports = factory();
  else root.FeedbackService = factory();
})(typeof self !== "undefined" ? self : this, function () {
  const TYPES = ["bug", "feature", "question", "other"];
  const EMAIL = /^[^@\s]+@[^@\s]+\.[^@\s]+$/;
  const VERSION = /^[0-9A-Za-z.+-]{1,32}$/;

  // Returns { ok, errors: {field: message}, clean }. clean has only schema fields.
  function validate(input) {
    const d = input || {};
    const errors = {};
    const type = String(d.type || "").trim().toLowerCase();
    const message = String(d.message || "").trim();
    const email = String(d.email || "").trim();
    const appVersion = String(d.version || d.appVersion || "").trim();
    if (!TYPES.includes(type)) errors.type = "Choose a type.";
    if (!message) errors.message = "Please write a message.";
    else if (message.length > 2000) errors.message = "Message is longer than 2000 characters.";
    if (email && (email.length > 254 || !EMAIL.test(email))) errors.email = "Email looks invalid.";
    if (appVersion && !VERSION.test(appVersion)) errors.version = "Version looks invalid (example: 0.1.0).";
    const clean = { type, message };
    if (email) clean.email = email;
    if (appVersion) clean.appVersion = appVersion;
    return { ok: Object.keys(errors).length === 0, errors, clean };
  }

  function randomId(cryptoImpl) {
    const c = cryptoImpl || (typeof crypto !== "undefined" ? crypto : null);
    const chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const bytes = new Uint8Array(20);
    if (c && c.getRandomValues) c.getRandomValues(bytes);
    else for (let i = 0; i < 20; i++) bytes[i] = Math.floor(Math.random() * 256);
    return Array.from(bytes, b => chars[b % chars.length]).join("");
  }

  // Firestore REST "commit" with a server-side timestamp, so createdAt is never
  // chosen by the browser. currentDocument.exists=false makes it a create.
  function buildCommit(clean, projectId, id) {
    const fields = {
      type: { stringValue: clean.type },
      message: { stringValue: clean.message },
      status: { stringValue: "new" }
    };
    if (clean.email) fields.email = { stringValue: clean.email };
    if (clean.appVersion) fields.appVersion = { stringValue: clean.appVersion };
    const name = `projects/${projectId}/databases/(default)/documents/feedback/${id}`;
    return {
      writes: [{
        update: { name, fields },
        updateMask: { fieldPaths: Object.keys(fields) },
        updateTransforms: [{ fieldPath: "createdAt", setToServerValue: "REQUEST_TIME" }],
        currentDocument: { exists: false }
      }]
    };
  }

  // cfg: { projectId, apiKey }. Resolves {ok:true} or {ok:false, reason}.
  async function submit(input, cfg, fetchFn, idFn) {
    const v = validate(input);
    if (!v.ok) return { ok: false, reason: "invalid", errors: v.errors };
    if (!cfg || !cfg.projectId || !cfg.apiKey) return { ok: false, reason: "not-configured" };
    const f = fetchFn || (typeof fetch !== "undefined" ? fetch : null);
    const url = `https://firestore.googleapis.com/v1/projects/${cfg.projectId}/databases/(default)/documents:commit?key=${encodeURIComponent(cfg.apiKey)}`;
    try {
      const r = await f(url, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(buildCommit(v.clean, cfg.projectId, (idFn || randomId)()))
      });
      return r.ok ? { ok: true } : { ok: false, reason: "http-" + r.status };
    } catch (e) {
      return { ok: false, reason: "network" };
    }
  }

  return { TYPES, validate, buildCommit, submit, randomId };
});
