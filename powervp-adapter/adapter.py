#!/usr/bin/env python

# Requires websocket and pyhdb packages

import websocket
import thread
import time
import json
import pyhdb

# Set HANA parameters
hanaHost = ""
hanaPort = 30015
hanaUser = ""
hanaPassword = ""

# Set PowerVP parameters
powervpGUIHost = ""
powervpSystemAgent = ""
powervpUser = ""
powervpPassword = ""
lpars = [4]


def insert_lpar(data):
    connection = pyhdb.connect(
        host=hanaHost,
        port=hanaPort,
        user=hanaUser,
        password=hanaPassword
    )

    cursor = connection.cursor()
    if 'graphLSUCPI' in data:
        columns = [data["lparID"], data["graphLSUCPI"], data["graphFXUCPI"], data["graphGCTCPI"], data["lsuL2CPI"], data["lsuL3CPI"], data["lsuL4CPI"], data["lsuMem"], data["lsuL21CPI"], data["lsuL31CPI"], data["lsuRL2L3CPI"], data["lsuRL4CPI"], data["lsuRMemCPI"], data["lsuDL2L3CPI"], data["lsuDL4CPI"], data["lsuDMemCPI"], data["elapsedTime"]]
        cursor.execute("""INSERT INTO "LPAR_SAMPLES"
                        ("LPAR", "LSU_CPI", "FXU_CPI", "GCT_CPI", "L2_CPI", "L3_CPI", "L4_CPI", "LOCAL_MEM_CPI", "L2.1_CPI", "L3.1_CPI", "REMOTE_L2L3_CPI", "REMOTE_L4_CPI", "REMOTE_MEM_CPI", "DISTANT_L2L3_CPI", "DISTANT_L4_CPI", "DISTANT_MEM_CPI", "ELAPSED_TIME")
                        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)""", columns)
    elif 'systemcpi' in data:
        system_columns = [data["systemcpi"], data['systemutil'], data['elapsedTime']]
        cursor.execute("""INSERT INTO "SYSTEM_SAMPLES"
                          ("SYSTEM_CPI", "SYSTEM_UTIL", "ELAPSED_TIME")
                          VALUES(?, ?, ?)""", system_columns)

        cursor.execute("""SELECT CURRENT_IDENTITY_VALUE() FROM "SYSTEM_SAMPLES" """)
        system_sample_id = cursor.fetchone()[0]

        for i in range(0, 95):
            core_util_columns = [system_sample_id, i, data['cpuUtils'][i], data['cpuOwners'][i]]
            cursor.execute("""INSERT INTO "CORE_UTIL_SAMPLES"
                              ("SYSTEM_SAMPLE_ID", "CORE", "UTIL", "OWNER")
                              VALUES(?, ?, ?, ?)""", core_util_columns)

    connection.commit()

def on_message(ws, message):
    message_json = json.loads(message)
    insert_lpar(message_json)

def on_error(ws, error):
    print error

def on_close(ws):
    print "### closed ###"

def on_open(ws):
    def run(*args):
        ws.send('{"action":"sysConnect","hostname":"' + powervpSystemAgent + '","username":"' + powervpUser + '","password":"' + powervpPassword + '","portvalue":"13000","sslportvalue":"13001","secure":"0","date":0,"time":"","cert":false}')
	for lpar in lpars:
            ws.send('{"action":"lparConnect","lparID":' + str(lpar) + ',"sysHost":"' + powervpSystemAgent + '","time":1483654131000,"historic":false}')
        for i in range(30000):
            time.sleep(1)
            ws.send('{"action":"sysData"}')
            for lpar in lpars:
                ws.send('{"action":"lparData","lparID":' + str(lpar) +'}')
        time.sleep(1)
        ws.close()
        print "thread terminating..."
    thread.start_new_thread(run, ())


if __name__ == "__main__":
    ws = websocket.WebSocketApp("ws://" + powervpGUIHost + ":9080/powerVPWeb/syslet",
                                on_message = on_message,
                                on_error = on_error,
                                on_close = on_close)
    ws.on_open = on_open

    ws.run_forever()
