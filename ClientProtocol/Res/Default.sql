# This table work with UserData class.
# Work with sqlite

CREATE TABLE IF NOT EXISTS users(
    id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    name VARCHAR(100) NOT NULL UNIQUE,
    pass VARCHAR(32) DEFAULT NULL,
    gmail VARCHAR(64) DEFAULT NULL UNIQUE,
    lastOnline date NOT NULL DEFAULT 0,
    onlinetime INTEGER NOT NULL DEFAULT 0,
    data BLOB DEFAULT NULL

);
