"""
Key/Value data store
"""

import sqlite3 as db
#from exceptions import KeyError

def init_parameters():
    con = db.connect('parameters.db')
    cur = con.cursor()

    cur.execute("DROP TABLE IF EXISTS topics")
    cur.execute("DROP TABLE IF EXISTS integers;")
    cur.execute("DROP TABLE IF EXISTS floats;")
    cur.execute("DROP TABLE IF EXISTS strings;")

    #cur.execute("CREATE TABLE topics(id INTEGER PRIMARY KEY, topic VARCHAR(64))")
    cur.execute("CREATE TABLE topics(key VARCHAR(64) PRIMARY KEY, value INT unique);")
    cur.execute("CREATE TABLE integers(key VARCHAR(64) PRIMARY KEY, value INT);")
    cur.execute("CREATE TABLE floats(key VARCHAR(64) PRIMARY KEY, value DOUBLE);")
    cur.execute("CREATE TABLE strings(key VARCHAR(64) PRIMARY KEY, value VARCHAR(256));")
    cur.execute("INSERT INTO integers(key, value) VALUES (?,?);", ["__MAX_PORT", 5556])

    con.commit()
    con.close()


def get_new_port(topic):
    topic_id = 0

    con = db.connect('parameters.db')
    cur = con.cursor()

    cur.execute("SELECT value FROM integers WHERE key = '__MAX_PORT';")
    values = cur.fetchall()
    topic_id = values[0][0] + 1
    cur.execute("INSERT INTO topics(key, value) VALUES(?,?)",[topic, topic_id])
    cur.execute("UPDATE integers SET value = ? WHERE key = '__MAX_PORT';", [topic_id])

    con.commit()

    return topic_id

def get_port(topic):
    topic_id = 0

    con = db.connect('parameters.db')
    cur = con.cursor()

    try:
        cur.execute("SELECT value FROM topics WHERE key = ?;", [topic])
    except db.OperationalError:
        init_parameters()
        cur.execute("SELECT value FROM topics WHERE key = ?;", [topic])

    values = cur.fetchall()
    if len(values) > 0:
        topic_id = values[0][0]
    else:
        topic_id = get_new_port(topic)

    return topic_id


def set_int_param(key, value):
    assert isinstance(value, int)

    con = db.connect('parameters.db')
    cur = con.cursor()
    cur.execute("INSERT OR REPLACE INTO integers (key, value) VALUES (?,?);",(key, value))

    con.commit()
    con.close()


def get_int_param(key):
    con = db.connect('parameters.db')
    cur = con.cursor()
    value = 0
    cur.execute("SELECT value FROM integers WHERE key = ?;", [key])
    rows = cur.fetchall()
    if len(rows) > 0:
        value = rows[0][0]
    else:
        raise KeyError
    return value


def set_float_param(key, value):
    assert isinstance(value, float)

    con = db.connect('parameters.db')
    cur = con.cursor()

    cur.execute("INSERT OR REPLACE INTO floats (key, value) VALUES (?,?);", (key, value))

    con.commit()
    con.close()


def get_float_param(key):
    con = db.connect('parameters.db')
    cur = con.cursor()
    value = 0.0
    cur.execute("SELECT value FROM floats WHERE key = ?;", [key])
    rows = cur.fetchall()
    if len(rows) > 0:
        value = rows[0][0]
    else:
        raise KeyError
    return value


def set_string_param(key, value):
    assert isinstance(value, str)

    con = db.connect('parameters.db')
    cur = con.cursor()

    cur.execute("INSERT OR REPLACE INTO strings (key, value) VALUES (?,?);", (key, value))

    con.commit()
    con.close()


def get_string_param(key):
    con = db.connect('parameters.db')
    cur = con.cursor()
    value = None
    cur.execute("SELECT value FROM strings WHERE key = ?;", [key])
    rows = cur.fetchall()
    if len(rows) > 0:
        value = rows[0][0]
    else:
        raise KeyError
    return value


if __name__ == '__main__':
    init_parameters()

    ## Topics:
    topicid1 = get_port("Test_Topic")
    topicid2 = get_port("Test_Topic")
    topicid3 = get_port("Other_Topic")

    assert topicid1 > 0
    assert topicid1 == topicid2
    assert topicid2 != topicid3

    ## Integers
    set_int_param("A", 1)
    set_int_param("B", 2)
    set_int_param("A", 3)

    assert get_int_param("A") == 3
    assert get_int_param("B") == 2

    try:
        get_int_param("XYZ")

        assert False, "Shouldn't get here, XYZ wasn't declared"

    except KeyError:
        pass

    ## Floats
    set_float_param("A", 1.1)
    set_float_param("B", 2.2)
    set_float_param("A", 3.3)

    assert get_float_param("A") == 3.3
    assert get_float_param("B") == 2.2

    try:
        get_float_param("XYZ")

        assert False, "Shouldn't get here, XYZ wasn't declared"

    except KeyError:
        pass

    ## Strings
    set_string_param("A", "1")
    set_string_param("B", "2")
    set_string_param("A", "3")

    assert get_string_param("A") == "3"
    assert get_string_param("B") == "2"

    try:
        get_string_param("XYZ")

        assert False, "Shouldn't get here, XYZ wasn't declared"

    except KeyError:
        pass
