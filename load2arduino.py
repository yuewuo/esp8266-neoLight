import paho.mqtt.client as mqtt

USERNAME = "sVyd16qv"

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("iot/" + USERNAME + "/#")
    
def on_message(client, userdata, msg):
    print(msg.topic + " : " + str(msg.payload))

client = mqtt.Client()
client.username_pw_set(USERNAME, "iIJQUrn5SkwpaCqG4omyW3Tb6z7RPvBY")
client.on_connect = on_connect
client.on_message = on_message
client.connect("onedawn.top", 8883, 60)

if __name__ == '__main__':

    # filename = input("输入txt文件名：")
    # with open(filename, 'rb') as f:
    #     client.publish("iot/" + USERNAME + "/neo/add", f.read())

    client.publish("iot/" + USERNAME + "/neo/add", 
        "1 default\64 0 10 0\n"
        + "fg:0 000000;10 000000\n"
        + "sg:800\n"
        + "fg:0 FF0000;10 0000FF\n"
        + "sg:1500\n"
        + "fg:0 00FF00;10 FF00FF\n"
        + "sg:800\n"
        + "fg:0 000000;10 000000\n"
    )

    # client.loop_forever()
