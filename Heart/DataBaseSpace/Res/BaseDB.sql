-- This is base table of network member (clients)
-- all additional information must be conteins in the another tables with links to the NetworkMembers table.
CREATE TABLE IF NOT EXISTS NetworkMembers (
    id VARCHAR(32) PRIMARY KEY,
    authenticationData BLOB NOT NULL,
    trust INTEGER default 0
);

-- The MemberPermisions table contains link to anothe database object and links to users of the database with permission lvl.
CREATE TABLE IF NOT EXISTS MemberPermisions (
    memberId VARCHAR(32) NOT NULL,
    dbAddress BLOB NOT NULL,
    lvl tinyint NOT NULL,

    FOREIGN KEY(memberId) REFERENCES NetworkMembers(id)
            ON UPDATE CASCADE
            ON DELETE CASCADE

);

CREATE UNIQUE INDEX IF NOT EXISTS MemberPermisionsIndex ON MemberPermisions(memberId, dbAddress);
