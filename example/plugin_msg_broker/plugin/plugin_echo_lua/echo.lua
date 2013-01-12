


print("lua load end ok...")

function handle_msg(channel_, msg_)
    channel.send(channel_, msg_)
    print(channel_, msg_)
end

function handle_broken(channel_)
    print(channel_)
end
