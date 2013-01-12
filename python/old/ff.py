# Experiment with wxPython's HtmlWindow
# tested with Python24 and wxPython26    vegaseat   17may2006
 #!/usr/bin/python
import wx
import wx.html
import os 


g_panel = None
g_frame = None
g_html_win = None

class ff_app(wx.PySimpleApp):
    def __init__(self):
        wx.PySimpleApp.__init__(self)
        os.chdir("./html")

class ff_html_win(wx.html.HtmlWindow):
    this = None
    def __init__(self, parent, pos = (0, 1), size = (600,400)):
        wx.html.HtmlWindow.__init__(self, parent, -1, pos, size)
        if "gtk2" in wx.PlatformInfo: 
            self.SetStandardFonts() 
    def even_handler(self, event):
        print ("even_handler:", event)


class ff_html_panel(wx.Panel):
    this = None
    """
    class ff_html_panel inherits wx.Panel and auto load files from html/index.html
    """
    def __init__(self, parent, id):
        # default pos is (0, 0) and size is (-1, -1) which fills the frame
        wx.Panel.__init__(self, parent, id)
        self.SetBackgroundColour("write")
        self.html_content = "<h2>404 Page Not found ./index.htm</h2>"
        self.html_win = ff_html_win(self, pos=(0,1), size=(800,600))
        global g_html_win
        g_html_win = self.html_win
        self.html_win.SetRelatedFrame(parent, parent.GetTitle() + " -- %s")
        self.html_win.SetRelatedStatusBar(0)
    def show(self):
        try:
            a = file("./index.html")
            self.html_content = a.read()
        except:
            pass

        self.html_win.SetPage(str(self.html_content))

    def even_handler(self, event):
        print ("even_handler:", event)

class ff_html_frame(wx.Frame):
    this = None
    def __init__(self, parent, title): 
        wx.Frame.__init__(self, parent, -1, title, size=(800,600)) 
        self.CreateStatusBar()
    def run(self):
        global g_panel 
        g_panel = ff_html_panel(self,-1)
        g_panel.show()
    def even_handler(self, event):
        print ("even_handler:", event)

class BlueTagHandler(wx.html.HtmlWinTagHandler):
    this = None
    def __init__(self):
        wx.html.HtmlWinTagHandler.__init__(self)

    def GetSupportedTags(self):
        return "BUTTON,INPUT,TEXTAREA"

    def HandleTag(self, tag):
        tag_name = tag.GetName()
        if tag_name == "INPUT":
            return self.HandleInput(tag)
        elif tag_name == "TEXTAREA":
            return self.HandleTextarea(tag)

    def HandleInput(self, tag):
        global g_html_win

        type_name = "text"
        if tag.GetParam("type") :
            type_name = tag.GetParam("type").lower()

        list = []
        if type_name == "text":
            inputTxtOne = wx.TextCtrl(g_html_win, wx.ID_ANY, tag.GetParam("value"))
            list.append(inputTxtOne)
        elif type_name == "button" or type_name == "submit":
            button = wx.Button(g_html_win, wx.ID_ANY, tag.GetParam("value"))
            #g_html_win.bind(wx.EVT_BUTTON, g_html_win.even_handler, button)
            list.append(button)

        for k in list:
            k.Show(True)
            self.GetParser().GetContainer().InsertCell(wx.html.HtmlWidgetCell(k, 0))
        self.ParseInner(tag)
        return True

    def HandleTextarea(self, tag):
        global g_html_win
        w = long(tag.GetParam("cols")) * 5
        h = long(tag.GetParam("rows")) * 20
        a = (w, h)
        print(a)
        tc = wx.TextCtrl(g_html_win, style=wx.TE_MULTILINE|wx.TE_READONLY, size=(w, h))
        self.GetParser().GetContainer().InsertCell(wx.html.HtmlWidgetCell(tc, 0))
        self.ParseInner(tag)
        return True

wx.html.HtmlWinParser_AddTagHandler(BlueTagHandler)

app = ff_app()
# create a window/frame, no parent, -1 is default ID, title, size
global g_frame
g_frame = ff_html_frame(None, "FFOWN HTML Browser") 
g_frame.run()

# call the derived class, -1 is default ID

# show the frame
g_frame.Show(True)
# start the event loop
app.MainLoop()

