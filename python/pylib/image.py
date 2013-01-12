import json
import urllib

def download_image(start = 0, num = 8):
    search_url = 'https://ajax.googleapis.com/ajax/services/search/images?v=1.0&start=' + str(start) + '&rsz=' + str(num) + '&q=beautifulgirl'
    print(search_url)
    search_ret = json.loads(urllib.urlopen(search_url).read())
    index = 0
    for k in search_ret["responseData"]["results"]:
        filename = k["titleNoFormatting"]
        url = k["url"]
        image_fd = urllib.urlopen(url)
        image = image_fd.read()

        dest = open("./html/download/" + str(start + index) + ".jpeg", "wb")
        dest.write(image)
        dest.close()
        index = index + 1

def download_image_from_sae(start = 0, num = 8):
    search_url = 'http://ffown.sinaapp.com/get_image_url.php'
    search_ret = json.loads(urllib.urlopen(search_url).read())
    index = start
    for url in search_ret:
        if index > num:
            break
        print(index, url)
        try:
            f = open("./download/" + str(index) + ".jpeg", "r")
            f.close()
            index = index + 1
            continue
        except:
            pass
        try:
            image_fd = urllib.urlopen(url)
            image = image_fd.read()
            dest = open("./html/download/" + str(index) + ".jpeg", "wb")
            dest.write(image)
            dest.close()
        except:
            pass
        index = index + 1

#download_image_from_sae()
