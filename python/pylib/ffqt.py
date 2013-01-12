from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4.QtWebKit import *
from PyQt4.QtNetwork import *
import os
import urllib
import random
import traceback

class BrowserScreen(QWebView):
    def __init__(self):
        QWebView.__init__(self)
        
        #self.setHtml(html_content)
        self.load(QUrl.fromLocalFile(os.getcwd() + "/index.html"))

        self.createTrayIcon()
        self.trayIcon.show()
        self.resize(800, 600)
        self.setWindowIcon(QIcon("images/own.ico"))
        self.setWindowTitle("FFOWN")
        self.show()
    def createTrayIcon(self):
        self.trayIcon = QSystemTrayIcon(self)
        self.trayIcon.setIcon(QIcon("images/xm.ico"))
    def showMessage(self, msg):
        self.trayIcon.showMessage("This is Python", msg,
            QSystemTrayIcon.MessageIcon(0), 15 * 1000)

class PythonJS(QObject):
    url = []
    __pyqtSignals__ = ("contentChanged(const QString &)")
    @pyqtSignature("", result="QString")
    def get_image_url(self):
        if len(self.url) == 0:
            search_url = 'http://ffown.sinaapp.com/get_image_url.php'
            search_ret = eval(urllib.urlopen(search_url).read())
            self.url = search_ret
        dest = self.url[random.randint(0, 100) % len(self.url)]
        return dest
    @pyqtSignature("QString", result="QString")
    def readfile(self, path):
	ret = ""
	try:
	    f = open(path, "r")
	    ret = f.read()
	    f.close()
	except:
		ret = "file not exist!" + path
		traceback.print_exc()
	return ret
    @pyqtSignature("QString", result="QString")
    def select_file(self, ext = "*"):
	ret = QFileDialog.getOpenFileName(None, "", ext, "FileDialog")
        return ret

def run_loop():
    import sys
    import os
    os.chdir("./html")
    app = QApplication(sys.argv)

    browser = BrowserScreen()
    pjs = PythonJS()
    browser.page().mainFrame().addToJavaScriptWindowObject("python", pjs)

    QObject.connect(pjs, SIGNAL("contentChanged(const QString &)"),
                    browser.showMessage)

    sys.exit(app.exec_())

    
