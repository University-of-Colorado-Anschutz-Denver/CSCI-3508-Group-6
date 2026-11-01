// We will set these once the repo and feedback backend exist.
const REPO = "OUR-USERNAME/OUR-REPO";            // owner/name on GitHub
const FEEDBACK_ENDPOINT = "";                       // e.g. a Cloud Function / Formspree URL

// Download page: show the latest release and link directly to its assets.
const verEl = document.getElementById("version");
if (verEl) {
  // Request the latest release from GitHub.
  fetch(`https://api.github.com/repos/${REPO}/releases/latest`)
    // Parse successful responses; treat HTTP errors as failures.
    .then(r => r.ok ? r.json() : Promise.reject())
    .then(rel => {
      // Show the release version on the page.
      verEl.textContent = rel.tag_name;
      // Connect Windows and Linux buttons to matching release files.
      for (const a of rel.assets || []) {
        if (a.name.endsWith(".exe")) document.getElementById("dl-win").href = a.browser_download_url;
        if (a.name.endsWith(".AppImage")) document.getElementById("dl-linux").href = a.browser_download_url;
      }
    })
    // Point visitors to GitHub Releases if loading fails.
    .catch(() => { verEl.textContent = "see GitHub Releases"; });
}

// Feedback form.
const form = document.getElementById("feedback-form");
if (form) {
  // Handle submissions without reloading the page.
  form.addEventListener("submit", async e => {
    e.preventDefault();
    // Find where to show validation and submission messages.
    const status = document.getElementById("form-status");
    // Collect the form fields into an object.
    const data = Object.fromEntries(new FormData(form));
    if (data.website) return;                        // honeypot: bots fill this
    // Require a message and a configured feedback service.
    if (!data.message || !data.message.trim()) { status.textContent = "Please write a message."; return; }
    if (!FEEDBACK_ENDPOINT) { status.textContent = "Feedback backend not connected yet."; return; }
    try {
      // Send the feedback to the configured service as JSON.
      const r = await fetch(FEEDBACK_ENDPOINT, { method: "POST", headers: { "Content-Type": "application/json" }, body: JSON.stringify(data) });
      if (!r.ok) throw new Error();
      // Clear the form and confirm it was sent.
      form.reset(); status.textContent = "Thanks! Your feedback was sent.";
    } catch {
      // Show an error if the service cannot accept the feedback.
      status.textContent = "Could not send. Please try again later.";
    }
  });
}
