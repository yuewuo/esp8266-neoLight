import paho.mqtt.client as mqtt

# USERNAME = "sVyd16qv"
# PASSWORD = "iIJQUrn5SkwpaCqG4omyW3Tb6z7RPvBY"
USERNAME = "KR9OlErL"
PASSWORD = "GKdl1LeOMDsPS8ykt3HfhF7J49iIpT2r"

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("iot/" + USERNAME + "/#")
    
def on_message(client, userdata, msg):
    print(msg.topic + " : " + str(msg.payload))

client = mqtt.Client()
client.username_pw_set(USERNAME, PASSWORD)
client.on_connect = on_connect
client.on_message = on_message
client.connect("onedawn.top", 8883, 60)

if __name__ == '__main__':

    # filename = input("输入txt文件名：")
    # with open(filename, 'rb') as f:
    #     client.publish("iot/" + USERNAME + "/neo/add", f.read())

    # client.publish("iot/" + USERNAME + "/neo/set/2", 
    #     "1 default\n64 0 60 -1\n"
    #     + "fg:0 000000;60 000000\n"
    #     + "sg:1500\n"
    #     + "fg:0 AAAA00;60 0000FF\n"
    #     + "sg:3000\n"
    #     + "fg:0 00FF00;60 FF0000\n"
    #     + "sg:1500\n"
    #     + "fg:0 000000;60 000000\n"
    #     # + "".join(["fg:0 00FF00;60 FF0000\n" for i in range(80)])
    # )

    # client.publish("iot/" + USERNAME + "/neo/set/2", "1 default\n64 0 60 -1\nfg:0 FF7300;60 FF7300\n")

    client.publish("iot/" + USERNAME + "/neo/set/2", "1 default\n64 0 60 -1\n"
        + "fg:0 000000;60 000000\n"
        + "sg:1500\n"
        + "fg:0 33FF66;30 000000;60 FF8833\n"
        + "sg:1500\n"
        + "fg:0 000000;60 000000\n"
    )
    
    client.publish("iot/" + USERNAME + "/neo/set/2", "1 default\n64 0 60 -1\n"
        + "fg:0 000000;60 000000\n"
        + "sg:1500\n"
        + "fg:0 663377;30 000000;60 BBFF33\n"
        + "sg:1500\n"
        + "fg:0 000000;60 000000\n"
    )

    # client.loop_forever()
