
CREATE TABLE IF NOT EXISTS Nodes (
    id VARCHAR(64) PRIMARY KEY NOT NULL,
    pubKey VARCHAR(256) NOT NULL,
    trust INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS NodesPermisions (
    nodeId VARCHAR(64) NOT NULL,
    objectTable VARCHAR(100) NOT NULL,
    objectId INTEGER NOT NULL,
    lvl INTEGER NOT NULL,

    FOREIGN KEY(nodeId) REFERENCES Nodes(id)
            ON UPDATE CASCADE
            ON DELETE CASCADE

);
ALTER TABLE NodesPermisions ADD UNIQUE INDEX (nodeId, objectTable, objectId);
