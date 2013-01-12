import channel
def handle_msg(channel_, msg_):
    print(channel_, msg_)
    channel.send(channel_, msg_)

def handle_broken(channel_):
    print(channel_)


print("load end ok ....")
