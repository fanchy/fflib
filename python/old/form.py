import wx

class MyForm(wx.Frame):

    def __init__(self):
        wx.Frame.__init__(self, None, wx.ID_ANY, 'My Form') #, size=(300,350))

        # Add a panel so it looks the correct on all platforms
        self.panel = wx.Panel(self, wx.ID_ANY)

        bmp = wx.ArtProvider.GetBitmap(wx.ART_INFORMATION, wx.ART_OTHER, (16, 16))
        titleIco = wx.StaticBitmap(self.panel, wx.ID_ANY, bmp)
        title = wx.StaticText(self.panel, wx.ID_ANY, 'My Title')

        bmp = wx.ArtProvider.GetBitmap(wx.ART_TIP, wx.ART_OTHER, (16, 16))
        inputOneIco = wx.StaticBitmap(self.panel, wx.ID_ANY, bmp)
        labelOne = wx.StaticText(self.panel, wx.ID_ANY, 'Input 1')
        inputTxtOne = wx.TextCtrl(self.panel, wx.ID_ANY, '')

        inputTwoIco = wx.StaticBitmap(self.panel, wx.ID_ANY, bmp)
        labelTwo = wx.StaticText(self.panel, wx.ID_ANY, 'Input 2')
        inputTxtTwo = wx.TextCtrl(self.panel, wx.ID_ANY, '')

        inputThreeIco = wx.StaticBitmap(self.panel, wx.ID_ANY, bmp)
        labelThree = wx.StaticText(self.panel, wx.ID_ANY, 'Input 3')
        inputTxtThree = wx.TextCtrl(self.panel, wx.ID_ANY, '')

        inputFourIco = wx.StaticBitmap(self.panel, wx.ID_ANY, bmp)
        labelFour = wx.StaticText(self.panel, wx.ID_ANY, 'Input 4')
        inputTxtFour = wx.TextCtrl(self.panel, wx.ID_ANY, '')

        okBtn = wx.Button(self.panel, wx.ID_ANY, 'OK')        
        cancelBtn = wx.Button(self.panel, wx.ID_ANY, 'Cancel')
        self.Bind(wx.EVT_BUTTON, self.onOK, okBtn)
        self.Bind(wx.EVT_BUTTON, self.onCancel, cancelBtn)

        topSizer        = wx.BoxSizer(wx.VERTICAL)
        titleSizer      = wx.BoxSizer(wx.HORIZONTAL)
        inputOneSizer   = wx.BoxSizer(wx.HORIZONTAL)
        inputTwoSizer   = wx.BoxSizer(wx.HORIZONTAL)
        inputThreeSizer = wx.BoxSizer(wx.HORIZONTAL)
        inputFourSizer  = wx.BoxSizer(wx.HORIZONTAL)
        btnSizer        = wx.BoxSizer(wx.HORIZONTAL)

        titleSizer.Add(titleIco, 0, wx.ALL, 5)
        titleSizer.Add(title, 0, wx.ALL, 5)
        
        inputOneSizer.Add(inputOneIco, 0, wx.ALL, 5)
        inputOneSizer.Add(labelOne, 0, wx.ALL, 5)
        inputOneSizer.Add(inputTxtOne, 1, wx.ALL|wx.EXPAND, 5)

        inputTwoSizer.Add(inputTwoIco, 0, wx.ALL, 5)
        inputTwoSizer.Add(labelTwo, 0, wx.ALL, 5)
        inputTwoSizer.Add(inputTxtTwo, 1, wx.ALL|wx.EXPAND, 5)
        
        inputThreeSizer.Add(inputThreeIco, 0, wx.ALL, 5)
        inputThreeSizer.Add(labelThree, 0, wx.ALL, 5)
        inputThreeSizer.Add(inputTxtThree, 1, wx.ALL|wx.EXPAND, 5)

        inputFourSizer.Add(inputFourIco, 0, wx.ALL, 5)
        inputFourSizer.Add(labelFour, 0, wx.ALL, 5)
        inputFourSizer.Add(inputTxtFour, 1, wx.ALL|wx.EXPAND, 5)

        btnSizer.Add(okBtn, 0, wx.ALL, 5)
        btnSizer.Add(cancelBtn, 0, wx.ALL, 5)

        topSizer.Add(titleSizer, 0, wx.CENTER)
        topSizer.Add(wx.StaticLine(self.panel,), 0, wx.ALL|wx.EXPAND, 5)
        topSizer.Add(inputOneSizer, 0, wx.ALL|wx.EXPAND, 5)
        topSizer.Add(inputTwoSizer, 0, wx.ALL|wx.EXPAND, 5)
        topSizer.Add(inputThreeSizer, 0, wx.ALL|wx.EXPAND, 5)
        topSizer.Add(inputFourSizer, 0, wx.ALL|wx.EXPAND, 5)
        topSizer.Add(wx.StaticLine(self.panel), 0, wx.ALL|wx.EXPAND, 5)
        topSizer.Add(btnSizer, 0, wx.ALL|wx.CENTER, 5)

        self.panel.SetSizer(topSizer)
        topSizer.Fit(self)
        

    def onOK(self, event):
        # Do something
        print 'onOK handler'
        
    def onCancel(self, event):
        self.closeProgram()

    def closeProgram(self):
        self.Close()


# Run the program
if __name__ == '__main__':
    app = wx.PySimpleApp()
    frame = MyForm().Show()
    app.MainLoop()
