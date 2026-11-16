# Feedback storage (Cloud Firestore)

The feedback form (`feedback.html`) writes one document per submission to the
`feedback` collection of a Firestore database, straight from the browser. No
server of our own runs. Safety comes from `firestore.rules`, which lets a
visitor **create** a valid record and nothing else.

Fields (match design section 4): `type` (bug | feature | question | other),
`message` (1-2000 chars), `email` (optional), `appVersion` (optional),
`createdAt` (server timestamp, never sent by the browser), `status` (always
`new` on create; the team changes it to `read` / `resolved` in the console).

## One-time setup (about 10 minutes)

1. Firebase console -> **Add project** (Spark/free plan is enough; no Cloud Functions are used).
2. **Build -> Firestore Database -> Create database**, production mode, pick a region.
3. **Project settings -> General -> Your apps -> Web app (`</>`)**. Copy `projectId` and `apiKey`.
4. Paste them into the `FIREBASE` constant near the top of `js/main.js`. The web API key is public by design; the rules protect the data.
5. Publish the rules, either:
   - paste `firebase/firestore.rules` into **Firestore -> Rules -> Publish**, or
   - `npm i -g firebase-tools && cd code/website/firebase && firebase login && firebase deploy --only firestore:rules --project <projectId>`
6. (Recommended) **Build -> App Check** or restrict the API key to your site's domain in Google Cloud console -> APIs & Services -> Credentials (HTTP referrers).
7. Submit a test message on the site, then open **Firestore -> feedback** to see it.

## Reviewing feedback (W-6)

Team members open the Firebase console -> Firestore -> `feedback`, sorted by `createdAt`.
Change `status` there. Only people with project access can read records; the website cannot.

## Retention

The design assumes records older than 12 months are deleted. Not automated yet. Option: add an
`expireAt` timestamp field and a Firestore TTL policy, or delete old records by hand each term.

## Test

`npm test` checks the validation, the request body and that the rules text denies everything except a valid create.
The rules themselves were not run against the Firestore emulator in our build environment;
use the console's **Rules playground** (or `firebase emulators:exec --only firestore`) once to confirm.
