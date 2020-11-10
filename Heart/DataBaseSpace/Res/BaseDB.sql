CREATE TABLE IF NOT EXISTS NetworkMembers (
    id INTEGER AUTO INCREMENT PRIMARY KEY NOT NULL ,
    authenticationData BLOB NOT NULL,
    trust INTEGER default 0,
    token BLOB default NULL
);

CREATE TABLE IF NOT EXISTS MemberPermisions (
    memberId INTEGER NOT NULL,
    dbAddress BLOB NOT NULL,
    lvl tinyint NOT NULL,

    FOREIGN KEY(memberId) REFERENCES NetworkMembers(id)
            ON UPDATE CASCADE
            ON DELETE CASCADE

);
CREATE UNIQUE INDEX IF NOT EXISTS MemberPermisionsIndex ON MemberPermisions(memberId, dbAddress);
