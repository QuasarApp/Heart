# AGENTS.md

Working guidance for AI coding agents and developers in the `QuasarApp/Heart` repository.

## Description

QuasarApp Heart is the base backend for C++/Qt projects: a library for
working with databases and light network requests, organized in two levels
(`AbstractNode` level and `DataBaseNode` level), with SSL socket support,
database initialization and data access. It is a submodule of
`QuasarApp/Games` (and transitively of Ecliptica).

- **Language:** C++17 + Qt (Core, Network, Sql, Concurrent; Qt6 or Qt5)
- **Build system:** CMake ≥ 3.19 (typically with Ninja)
- **License:** LGPLv3

### Repository layout

| Path | Contents |
|---|---|
| `src/` | the `Heart` library (nodes, database, network, `public/` + `private/`) |
| `HeartTests/` | Qt Test suite (`HeartTest`: abstractnode, bigdata, …) |
| `res/` | resources (banner, …) |
| `submodules/` | `QuasarAppLib`, `crc`, `easyssl`, `constexpr-xxh3` |

### Architecture

- `AbstractNode` — the async/network base (`QH::Async`,
  `AsyncRenderLoop`, `StreamBase`/`StreamMultiversion`, `Package`/
  `AbstractData`); `DataBaseNode` adds SQL storage on top.
- `HEART_SSL=ON` (default on Linux) pulls in the `easyssl` submodule for
  SSL sockets.
- The QuasarApp CMake layer (`QuasarAppLib/CMake/QuasarApp.cmake`)
  provides the `test` / `testHeartTest` / `deploy` / `doc` targets.

## Developing process

### Toolchain

| Tool | Version / notes |
|---|---|
| CMake | ≥ 3.19 |
| Compiler | C++17 |
| Qt | 6.10 (Qt 5 supported), modules: Core, Network, Sql, Concurrent, Test |
| Ninja | preferred generator |
| OpenSSL | for SSL sockets (`HEART_SSL=ON`) |
| CQtDeployer | `cqtdeployer` on `PATH` — required by the `deploy`/`testHeartTest` targets |

### Build

Step by step.

**1. One-time toolchain setup.** Qt 6.10.3 via aqtinstall
(`aqt install-qt linux desktop 6.10.3 gcc_64 -O /opt/Qt`) plus CMake ≥ 3.19,
Ninja and the OpenSSL dev packages.

**2. Get the sources.** Submodules are mandatory (recursive:
`QuasarAppLib`, `crc`, `easyssl`, `constexpr-xxh3`):

```sh
git clone --recurse-submodules https://github.com/QuasarApp/Heart.git
# or, for an existing clone:
git submodule update --init --recursive
```

**3. Configure.** (`HEART_TESTS` is `ON` by default on desktop platforms.)

```sh
cmake -B build -G Ninja \
  -DCMAKE_PREFIX_PATH=/opt/Qt/6.10.3/gcc_64 \
  -DBUILD_SHARED_LIBS=ON \
  -DCMAKE_BUILD_TYPE=Debug \
  -DHEART_TESTS=ON
```

**4. Build.**

```sh
cmake --build build --parallel 16
```

**5. Find the result.** The suite executable lands in its per-target build
dir (`build/HeartTests/HeartTest`); the deliverable is the `Heart` library
(`libHeart`).

CMake options (top level):

- `HEART_TESTS` — build the test suite (default `ON`; forced `OFF` on Android/iOS)
- `HEART_SSL` — SSL sockets via easyssl (default `ON` on Linux, `OFF` on Windows)
- `HEART_STATIC_SSL` — statically link the SSL libraries (default `OFF`)
- `HEART_PRINT_PACKAGES` — log incoming network packages (default `OFF`)
- `HEART_PRINT_SQL_QUERIES` — log all SQL queries (default `OFF`)
- `HEART_VALIDATE_PACKS` — validate child classes of `DataPack` (default `ON`)

### Branching, labels and pull requests (workflow for AI agents)

- Work is issue-driven: one GitHub issue → one branch named
  `task_<issue-number>`, branched **strictly from `main`** — never from
  another task branch.
- Commit messages: short imperative subject + issue reference, e.g.
  `fix: <summary> (issue #<issue>)`, `docs: <summary> (issue #<issue>)`.
- Keep the build and lint green; a red CI blocks the merge.

**The exact commands.**

```sh
# 1. every task starts from a freshly fetched main
git fetch origin
git checkout -b task_<issue> origin/main

# 2. label the issue while working. Workflow labels in this repo:
#    in-progress, waiting-for-review, help wanted, question, bug
gh issue edit <issue> --add-label in-progress

# 3. post the initial checklist as a comment; afterwards update that same
#    comment in place, never re-post it
gh issue comment <issue> --body-file /tmp/checklist.md
gh issue comment <issue> --edit-last --body-file /tmp/checklist.md

# 4. push and open the PR — see "Creating the pull request" below
git push -u origin task_<issue>
gh pr create --base main --head task_<issue> \
  --title "<type>: <short imperative summary> (issue #<issue>)" \
  --body-file /tmp/pr_body.md

# 5. the issue goes to waiting-for-review; watch CI from the CLI
gh issue edit <issue> --remove-label in-progress --add-label waiting-for-review
gh pr checks <number>
```

This repo does not define a `blocked` label yet — create it before first
use: `gh label create blocked --color 'e6e6e6'`.

**Status messages — what to post and when.** An agent must keep a
human-readable trail on the issue (and on the PR once it exists). Every
message is a GitHub comment with a `##` heading; the checklist comment is
updated in place (`--edit-last`), never duplicated.

| Stage | Label(s) | Comment template |
|---|---|---|
| Start | `in-progress` | `## Task issue_XXX: <title> — initial checklist (branch task_XXX from main)` — one `- [ ]` line per sub-task |
| Each step done | — | edit the checklist comment: mark the line `- [x]` + one short evidence note (command run, result) |
| Blocked / needs input | `blocked` / `question` | `## Blocked — task_XXX` — what is blocking and exactly what is needed to continue |
| PR opened | `waiting-for-review` | `## Progress — task_XXX` — all lines `- [x]` + `**PR: <url>**` + one-line verification summary |
| Review changes requested | `waiting-for-review` | `## Review-fix cycle — PR #NNN — update` — one `- [x]` line per review point, each with the evidence |
| CI red on the PR | `in-progress` | `## CI failure — PR #NNN` — failing check, cause, fix plan; push the fix, then update the comment |

**Creating the pull request.** An agent has no browser and cannot create a
pull request from the GitHub web UI — it must use the `gh` CLI (a token with
the `repo` scope; in this organisation the agent runs as `QuasarAppCI`):

```sh
gh pr create \
  --base main \
  --head task_<issue> \
  --title "<type>: <short imperative summary> (issue #<issue>)" \
  --body-file /tmp/pr_body.md
```

- The body carries what changed and the verification actually performed
  (commands + their real output); `(issue #<n>)` in the title links the PR
  to the issue. Use `--draft` while still iterating.
- A pull request exists only when `gh pr create` prints its URL. Confirm
  with `gh pr view <number> --json number,url,mergeable` before reporting
  "PR created" — never post that status on faith.
- If work is blocked, keep the PR open, set the issue label to `blocked` and
  post the status message above.

### Documentation rules

- The README documents the node levels and features — keep it in sync with
  the API in `src/public/`.
- A new node level / package type ships **with** a unit test in
  `HeartTests/units/` — never in a follow-up ticket.

## Testing process

### Test infrastructure

Qt Test suite in `HeartTests/` (`tst_testprotockol.cpp` +
`HeartTests/units/`): the executable `HeartTest` links `Qt::Test` and
`Heart` (Qt 6 or Qt 5); built with `HEART_TESTS=ON` (default on desktop,
forced `OFF` on Android/iOS).

The `testProtockol` class registers the units in `tst_testprotockol.cpp`
with the `TestCase(name, testClass)` macro:

| Unit | Covers |
|---|---|
| `abstractNodeTest` | node connect + package round-trip over local TCP (port 27778) |
| `bigDataTest` | 10 MB multi-part big-data transfer between two nodes (port 27781) |
| `shedullerTest` | the node sheduler / periodic send (slowest unit, ~11 s) |
| `upgradeDataBaseTest` | SQLite schema upgrades 0→1→2→3 and re-open |
| `multiVersionTest` | multiversion stream protocol |

The QuasarApp CMake layer (`submodules/QuasarAppLib/CMake/QuasarAppCITargets.cmake`,
included from `QuasarApp.cmake`) registers the `test` aggregate plus
`deployTestHeartTest` (cqtdeployer deploys `build/HeartTests/HeartTest` into
`<repo>/Testing/HeartTest/`) and `testHeartTest` (executes
`Testing/HeartTest/HeartTest.sh` from that directory), wired in with
`add_dependencies(test testHeartTest)`.

### Running the tests from CMake

```sh
cmake -B build -G Ninja -DCMAKE_PREFIX_PATH=/opt/Qt/6.10.3/gcc_64 \
  -DCMAKE_BUILD_TYPE=Debug -DHEART_TESTS=ON
cmake --build build --parallel 16
```

Then run the suite executable directly (the build rpath already resolves the
Qt, OpenSSL and `Heart` libraries):

```sh
./build/HeartTests/HeartTest
```

or via the CMake test targets — the path CI uses; requires `cqtdeployer`
on `PATH` (deploys into `Testing/HeartTest/` first, then executes the
generated `HeartTest.sh`):

```sh
cmake --build build --target test
```

Expected on `main` (verified, ~12 s, headless):
`Totals: 7 passed, 0 failed, 0 skipped, 0 blacklisted` — `initTestCase`,
the five units above and `cleanupTestCase`.

To run a single unit, pass its test-function name as a positional argument
(note: in Qt 6 the `-functions` flag only *lists* the functions — the
Qt 5-style `-functions <name>` no longer selects one):

```sh
./build/HeartTests/HeartTest bigDataTest
```
