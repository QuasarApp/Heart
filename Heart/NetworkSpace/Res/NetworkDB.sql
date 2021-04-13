CREATE TABLE IF NOT EXISTS NetworkMembers (
    id VARCHAR(64) PRIMARY KEY NOT NULL ,
    authenticationData BLOB NOT NULL,
    trust INTEGER default 0
);

CREATE TABLE IF NOT EXISTS MemberPermisions (
    memberId VARCHAR(64) NOT NULL,
    dbAddress BLOB NOT NULL,
    lvl tinyint NOT NULL,

    FOREIGN KEY(memberId) REFERENCES NetworkMembers(id)
            ON UPDATE CASCADE
            ON DELETE CASCADE

);
CREATE UNIQUE INDEX IF NOT EXISTS MemberPermisionsIndex ON MemberPermisions(memberId, dbAddress);
