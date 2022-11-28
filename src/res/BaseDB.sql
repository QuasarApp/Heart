-- This is base table of network member (clients)
-- all additional information must be conteins in the another tables with links to the NetworkMembers table.

CREATE TABLE IF NOT EXISTS DataBaseAttributes (
    name TEXT NOT NULL PRIMARY KEY,
    value INT NOT NULL UNIQUE
);


