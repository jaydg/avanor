# Signing the releases

This is for whoever publishes Avanor. Everything it describes is already
built and already in CI; none of it does anything until the accounts
below exist and their secrets are in the repository settings. Until then
every release is produced exactly as it is today, unsigned, and the
disk image tells the user what macOS will say about that.

Nothing here needs to be done in a hurry, and nothing here needs to be
done at all if the answer is that it is not worth ninety-nine dollars a
year. Read the first section and decide.

## What this buys, and what it does not

**macOS.** Today, opening Avanor for the first time fails: *"Avanor" Not
Opened*. The only way through is System Settings, Privacy & Security,
scroll to the bottom, Open Anyway - macOS 15 withdrew the old
Control-click shortcut, so there is no quicker route to point people at.
Signing and notarising removes that entirely. The application opens on a
double click, with no dialogue at all. This is a complete fix, and it is
the reason to spend the money.

**Windows.** Today SmartScreen says "Windows protected your PC" and names
the publisher as unknown. Signing replaces *unknown publisher* with a
real name, which is most of the benefit. It does **not** make the prompt
go away immediately: since a policy change in 2024 a certificate no
longer buys an exemption, only a reputation, and that accrues as people
download the installer. Expect the prompt to persist for a while and then
stop. This part is free, so the only cost is the application effort.

## What it costs

| | Cost | Notes |
| --- | --- | --- |
| Apple Developer Program | **$99/year** | The whole of the macOS cost |
| Developer ID certificate | included | Comes with the membership |
| App Store Connect API key | free | What CI notarises with |
| Notarisation | free | No limit worth worrying about |
| SignPath Foundation | **free** | Windows, for open source |

So: **$99 a year, and nothing else.** Both alternatives to SignPath are
worse for this project - Azure Trusted Signing is about $10 a month and
is limited to organisations in the US and Canada, and a commercial
certificate is $200-600 a year and, since the key-storage rules changed
in 2023, has to live on a hardware token, which is its own problem for a
build that runs on someone else's machine.

## Apple, part one: the membership

Enrol at <https://developer.apple.com/programs/enroll/>.

**Enrol as an Individual, not as an Organization.** They cost the same.
An Organization has to supply a D-U-N-S number registered to a legal
entity, which Avanor does not have and would have to invent a company to
get. An Individual needs none. The only visible difference is that the
certificate, and so the name macOS shows, is a personal name rather than
a project name.

Identity verification takes days rather than minutes, so start it before
you need it. The membership renews annually; see the last section for
what lapsing does.

## Apple, part two: the certificate

1. In the Apple Developer account, under Certificates, create a
   certificate of type **Developer ID Application**. Not "Mac
   Development", not "Developer ID Installer" - only the Application one
   signs an application and a disk image.
2. Download it and open it, which puts it in the login keychain.
3. In Keychain Access, find it, and export **the certificate together
   with its private key** as a `.p12`. It will ask for a password; make
   one up and keep it, it becomes a secret below.
4. Turn it into text, because that is what a GitHub secret holds:

   ```sh
   base64 -i Certificates.p12 | pbcopy
   ```

   That is `MACOS_CERTIFICATE`. The password from step 3 is
   `MACOS_CERTIFICATE_PASSWORD`.

## Apple, part three: the notarisation key

Notarisation is a separate conversation with Apple, and CI does it with
an App Store Connect API key rather than with an Apple ID and password.
The key is not tied to a person, survives two-factor prompts, and can be
revoked on its own.

1. In App Store Connect, under Users and Access, Integrations, Keys,
   generate a **team key**. Developer access is enough to notarise.
2. Download the `.p8`. **It downloads once.** If it is lost, revoke it
   and make another.
3. Note the **Key ID** beside it, and the **Issuer ID** shown above the
   list - the issuer is one value for the whole team, not per key.
4. As before:

   ```sh
   base64 -i AuthKey_XXXXXXXXXX.p8 | pbcopy
   ```

   That is `NOTARY_KEY`; the other two are `NOTARY_KEY_ID` and
   `NOTARY_ISSUER`.

## Windows: SignPath Foundation

SignPath Foundation signs open source projects at no cost. Avanor
qualifies: the repository is public and the licence is GPLv2+. The
certificate is issued to the Foundation and its private key stays in
their hardware, so nothing sensitive is ever in this repository - CI
uploads the built installer and gets a signed one back.

Apply through <https://signpath.org/>. When the project is approved they
provide an organisation id, a project slug and a signing policy slug.
The token is a secret; the other three are not, and go in repository
**variables** rather than secrets.

## The secrets, in one place

Repository Settings, Secrets and variables, Actions.

**Secrets:**

| Name | What it holds | From |
| --- | --- | --- |
| `MACOS_CERTIFICATE` | the `.p12`, base64 | Apple, part two |
| `MACOS_CERTIFICATE_PASSWORD` | the password you chose for it | Apple, part two |
| `NOTARY_KEY` | the `.p8`, base64 | Apple, part three |
| `NOTARY_KEY_ID` | the key's id | Apple, part three |
| `NOTARY_ISSUER` | the team's issuer id | Apple, part three |
| `SIGNPATH_API_TOKEN` | the SignPath token | SignPath |

**Variables:**

| Name | What it holds |
| --- | --- |
| `SIGNPATH_ORGANIZATION_ID` | from SignPath |
| `SIGNPATH_PROJECT_SLUG` | from SignPath |
| `SIGNPATH_POLICY_SLUG` | from SignPath |

They are independent. The macOS six and the Windows four can be added
months apart, and each platform starts signing as soon as its own are
there. Adding none of them leaves everything working as it does now.

## Checking that it worked

Push a tag, then download the disk image from the release the way a
stranger would - through a browser, not with `curl`, so that it carries
the quarantine flag that makes macOS care.

```sh
spctl -a -vvv -t exec /Applications/Avanor.app     # wanted: accepted
xcrun stapler validate /Applications/Avanor.app    # the app, not just the image
codesign -dvv /Applications/Avanor.app             # wanted: a Developer ID, a TeamIdentifier
```

`spctl` saying `accepted` and a `TeamIdentifier` that is not `not set`
are the two that matter. The honest test is the one that needs no
commands: double-click it and see nothing happen except the game
starting.

Two things worth knowing before you debug something that is not broken:

- **Do not test this with ad-hoc signing** (`codesign --sign -`). The
  hardened runtime turns on library validation, which requires the
  bundled libraries and the game to carry the *same* team, and an ad-hoc
  signature has no team at all. It fails with *"mapping process and
  mapped file (non-platform) have different Team IDs"*, which looks
  alarming and means only that you signed with a dash. A real Developer
  ID signs all four with one team and the problem does not exist.
- **Check the Apple silicon image in particular.** The two entitlements
  in `resources/avanor.entitlements` exist because the hardened runtime
  otherwise forbids LuaJIT from running what it compiles. On Intel this
  was tested and the game runs either way; arm64 enforces it far more
  strictly, and if it were ever going to bite it would bite there. The
  symptom to watch for is not a crash but sluggishness, because the
  refusal stops the compiler and not the game.

## Renewal, expiry and loss

The membership renews every year. **Let it lapse and new releases stop
being signed** - the build does not fail, it quietly produces the
unsigned image again, which is the right behaviour but is easy not to
notice. Releases already published are unaffected: a notarised, stapled
copy keeps working forever, because the ticket travels with it and does
not get re-checked against a live certificate.

The Developer ID certificate itself is good for five years. Renewing it
means repeating "Apple, part two" and replacing two secrets.

If the `.p12` or the `.p8` ever leaks, revoke it in the Apple developer
account first and replace the secret afterwards. Revoking does not
invalidate what was already notarised.
