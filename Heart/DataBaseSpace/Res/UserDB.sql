-- This is base table of network member (users)
-- All users have a own internal id is integer value for impruve a database performance and userName.
-- userName is unique fiels for login and execute select queries (for get user information without user id).
-- all additional information must be conteins in the another tables with links to the NetworkMembers table.
CREATE TABLE IF NOT EXISTS NetworkMembers (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    userName VARCHAR(64) UNIQUE NOT NULL,
    authenticationData BLOB NOT NULL,
    trust INTEGER default 0,
    token BLOB default NULL
);

CREATE UNIQUE INDEX IF NOT EXISTS NetworkMembersIndex ON NetworkMembers(userName);

-- The MemberPermisions table contains link to anothe database object and links to users of the database with permission lvl.
CREATE TABLE IF NOT EXISTS MemberPermisions (
    memberId INTEGER NOT NULL,
    dbAddress BLOB NOT NULL,
    lvl tinyint NOT NULL,

    FOREIGN KEY(memberId) REFERENCES NetworkMembers(id)
            ON UPDATE CASCADE
            ON DELETE CASCADE

);

CREATE UNIQUE INDEX IF NOT EXISTS MemberPermisionsIndex ON MemberPermisions(memberId, dbAddress);
