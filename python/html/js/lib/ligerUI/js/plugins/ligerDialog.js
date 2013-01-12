/**
* jQuery ligerUI 1.1.5
* 
* Author leoxie [ gd_star@163.com ] 
* 
*/

(function ($)
{
    $.ligerDialog = function ()
    {
        return $.ligerui.run.call(null, "ligerDialog", arguments, { isStatic: true });
    };

    //dialog 图片文件夹的路径 针对于IE6设置 
    $.ligerui.DialogImagePath = "../../lib/ligerUI/skins/Aqua/images/dialog/";

    $.ligerDefaults.Dialog = {
        cls: null,       //给dialog附加css class
        id: null,        //给dialog附加id
        buttons: null, //按钮集合 
        isDrag: true,   //是否拖动
        width: 280,     //宽度
        height: null,   //高度，默认自适应 
        content: '',    //内容
        target: null,   //目标对象，指定它将以appendTo()的方式载入
        url: null,      //目标页url，默认以iframe的方式载入
        load: false,     //是否以load()的方式加载目标页的内容
        onLoaded: null,
        type: 'none',   //类型 warn、success、error、question
        left: null,     //位置left
        top: null,      //位置top
        modal: true,    //是否模态对话框
        name: null,     //创建iframe时 作为iframe的name和id 
        isResize: false, // 是否调整大小
        allowClose: true, //允许关闭
        opener: null,
        timeParmName: null,  //是否给URL后面加上值为new Date().getTime()的参数，如果需要指定一个参数名即可
        closeWhenEnter: null, //回车时是否关闭dialog
        isHidden: true,        //关闭对话框时是否只是隐藏，还是销毁对话框
        show: true,          //初始化时是否马上显示
        title: null        //头部
    };
    $.ligerDefaults.DialogString = {
        titleMessage: '提示',                     //提示文本标题
        waittingMessage: '正在等待中,请稍候...'
    };

    $.ligerMethos.Dialog = $.ligerMethos.Dialog || {};


    $.ligerui.controls.Dialog = function (options)
    {
        $.ligerui.controls.Dialog.base.constructor.call(this, null, options);
    };
    $.ligerui.controls.Dialog.ligerExtend($.ligerui.core.UIComponent, {
        __getType: function ()
        {
            return 'Dialog';
        },
        __idPrev: function ()
        {
            return 'Dialog';
        },
        _extendMethods: function ()
        {
            return $.ligerMethos.Dialog;
        },
        _render: function ()
        {
            var g = this, p = this.options;
            var dialog = $('<div class="l-dialog"><table class="l-dialog-table" cellpadding="0" cellspacing="0" border="0"><tbody><tr><td class="l-dialog-tl"></td><td class="l-dialog-tc"><div class="l-dialog-tc-inner"><div class="l-dialog-icon"></div><div class="l-dialog-title"></div><div class="l-dialog-close"></div></div></td><td class="l-dialog-tr"></td></tr><tr><td class="l-dialog-cl"></td><td class="l-dialog-cc"><div class="l-dialog-body"><div class="l-dialog-image"></div> <div class="l-dialog-content"></div><div class="l-dialog-buttons"><div class="l-dialog-buttons-inner"></div></td><td class="l-dialog-cr"></td></tr><tr><td class="l-dialog-bl"></td><td class="l-dialog-bc"></td><td class="l-dialog-br"></td></tr></tbody></table></div>');
            $('body').append(dialog);
            g.dialog = dialog;
            g.element = dialog[0];
            g.dialog.body = $(".l-dialog-body:first", g.dialog);

            g.set(p);

            if (p.allowClose == false) $(".l-dialog-close", g.dialog).remove();
            if (p.target || p.url || p.type == "none") p.type = null;
            if (p.cls) g.dialog.addClass(p.cls);
            if (p.id) g.dialog.attr("id", p.id);
            //设置锁定屏幕、拖动支持 和设置图片
            if (p.modal)
                g._applyWindowMask();
            if (p.isDrag)
                g._applyDrag();
            if (p.isResize)
                g._applyResize();
            if (p.type)
                g._setImage();
            else
            {
                $(".l-dialog-image", g.dialog).remove();
                $(".l-dialog-content", g.dialog.body).addClass("l-dialog-content-noimage");
            }
            if (!p.show)
            {
                if (g.windowMask) g.windowMask.hide();
                g.dialog.hide();
            }
            //设置主体内容
            if (p.target)
            {
                $(".l-dialog-content", g.dialog.body).prepend(p.target);
            }
            else if (p.url)
            {
                if (p.timeParmName)
                {
                    p.url += p.url.indexOf('?') == -1 ? "?" : "&";
                    p.url += p.timeParmName + "=" + new Date().getTime();
                }
                if (p.load)
                {
                    g.dialog.body.load(p.url, function ()
                    {
                        g.trigger('loaded');
                    });
                }
                else
                {
                    g.jiframe = $("<iframe frameborder='0'></iframe>");
                    var framename = p.name ? p.name : "ligerwindow" + new Date().getTime();
                    g.jiframe.attr("name", framename);
                    g.jiframe.attr("id", framename);
                    $(".l-dialog-content", g.dialog.body).prepend(g.jiframe);
                    $(".l-dialog-content", g.dialog.body).addClass("l-dialog-content-nopadding");
                    setTimeout(function ()
                    {
                        g.jiframe.attr("src", p.url);
                        g.frame = window.frames[g.jiframe.attr("name")];
                    }, 0);
                }
            }
            else if (p.content)
            {
                $(".l-dialog-content", g.dialog.body).html(p.content);
            }
            if (p.opener) g.dialog.opener = p.opener;
            //设置按钮
            if (p.buttons)
            {
                $(p.buttons).each(function (i, item)
                {
                    var btn = $('<div class="l-dialog-btn"><div class="l-dialog-btn-l"></div><div class="l-dialog-btn-r"></div><div class="l-dialog-btn-inner"></div></div>');
                    $(".l-dialog-btn-inner", btn).html(item.text);
                    $(".l-dialog-buttons-inner", g.dialog.body).prepend(btn);
                    item.width && btn.width(item.width);
                    item.onclick && btn.click(function () { item.onclick(item, g, i) });
                });
            } else
            {
                $(".l-dialog-buttons", g.dialog).remove();
            }
            $(".l-dialog-buttons-inner", g.dialog).append("<div class='l-clear'></div>");

            //设置参数属性
            p.width && g.dialog.body.width(p.width - 26);
            if (p.height)
            {
                $(".l-dialog-content", g.dialog.body).height(p.height - 46 - $(".l-dialog-buttons", g.dialog).height());
            }
            p.title = p.title || p.titleMessage;
            p.title && $(".l-dialog-title", g.dialog).html(p.title);
            $(".l-dialog-title", g.dialog).bind("selectstart", function () { return false; });


            //设置事件
            $(".l-dialog-btn", g.dialog.body).hover(function ()
            {
                $(this).addClass("l-dialog-btn-over");
            }, function ()
            {
                $(this).removeClass("l-dialog-btn-over");
            });
            $(".l-dialog-tc .l-dialog-close", g.dialog).hover(function ()
            {
                $(this).addClass("l-dialog-close-over");
            }, function ()
            {
                $(this).removeClass("l-dialog-close-over");
            }).click(function ()
            {
                if (p.isHidden)
                    g.hidden();
                else
                    g.close();
            });
            var ie55 = $.browser.msie && $.browser.version == "5.5";
            var ie6 = $.browser.msie && $.browser.version == "6.0";

            if ($.browser.msie && (ie55 || ie6))
            {
                $(".l-dialog-tl:first", g.dialog).css({
                    "background": "none",
                    "filter": "progid:DXImageTransform.Microsoft.AlphaImageLoader(src='" + $.ligerui.DialogImagePath + "dialog-tl.png',sizingMethod='crop');"
                });
                $(".l-dialog-tc:first", g.dialog).css({
                    "background": "none",
                    "filter": "progid:DXImageTransform.Microsoft.AlphaImageLoader(src='" + $.ligerui.DialogImagePath + "ie6/dialog-tc.png',sizingMethod='crop');"
                });
                $(".l-dialog-tr:first", g.dialog).css({
                    "background": "none",
                    "filter": "progid:DXImageTransform.Microsoft.AlphaImageLoader(src='" + $.ligerui.DialogImagePath + "dialog-tr.png',sizingMethod='crop');"
                });
                $(".l-dialog-cl:first", g.dialog).css({
                    "background": "none",
                    "filter": "progid:DXImageTransform.Microsoft.AlphaImageLoader(src='" + $.ligerui.DialogImagePath + "ie6/dialog-cl.png',sizingMethod='crop');"
                });
                $(".l-dialog-cr:first", g.dialog).css({
                    "background": "none",
                    "filter": "progid:DXImageTransform.Microsoft.AlphaImageLoader(src='" + $.ligerui.DialogImagePath + "ie6/dialog-cr.png',sizingMethod='crop');"
                });
                $(".l-dialog-bl:first", g.dialog).css({
                    "background": "none",
                    "filter": "progid:DXImageTransform.Microsoft.AlphaImageLoader(src='" + $.ligerui.DialogImagePath + "dialog-bl.png',sizingMethod='crop');"
                });
                $(".l-dialog-bc:first", g.dialog).css({
                    "background": "none",
                    "filter": "progid:DXImageTransform.Microsoft.AlphaImageLoader(src='" + $.ligerui.DialogImagePath + "ie6/dialog-bc.png',sizingMethod='crop');"
                });
                $(".l-dialog-br:first", g.dialog).css({
                    "background": "none",
                    "filter": "progid:DXImageTransform.Microsoft.AlphaImageLoader(src='" + $.ligerui.DialogImagePath + "dialog-br.png',sizingMethod='crop');"
                });
            }
            //位置初始化
            var left = 0;
            var top = 0;
            var width = p.width || g.dialog.width();
            if (p.left != null) left = p.left;
            else left = 0.5 * ($(window).width() - width);
            if (p.top != null) top = p.top;
            else top = 0.5 * ($(window).height() - g.dialog.height()) + $(window).scrollTop() - 10;
            if (left < 0) left = 0;
            if (top < 0) top = 0;
            g.dialog.css({ left: left, top: top });
            g.dialog.show();
            $('body').bind('keydown.dialog', function (e)
            {
                var key = e.which;
                if (key == 13)
                {
                    g.enter();
                }
                else if (key == 27)
                {
                    g.esc();
                }
            });
        },
        //按下回车
        enter: function ()
        {
            var g = this; var po = this.po;
            var isClose;
            if (p.closeWhenEnter != undefined)
            {
                isClose = p.closeWhenEnter;
            }
            else if (p.type == "warn" || p.type == "error" || p.type == "success" || p.type == "question")
            {
                isClose = true;
            }
            if (isClose)
            {
                g.close();
            }
        },
        esc: function ()
        {

        },
        close: function ()
        {
            var g = this; var po = this.po;
            if (g.frame)
            {
                $(g.frame.document).ready(function ()
                {
                    if (g.windowMask) g.windowMask.remove();
                    g.dialog.remove();
                });
            }
            else
            {
                if (g.windowMask) g.windowMask.remove();
                g.dialog.remove();
            }
            $('body').unbind('keydown.dialog');
        },
        set: function (options)
        {
            if (!options) return;
            var g = this; var p = this.options;
            if (options.title)
            {
                $(".l-dialog-title", g.dialog).html(options.title);
            }
            if (options.url)
            {
                g.setUrl(options.url);
            }
            $.extend(p, options);
        },
        hidden: function ()
        {
            var g = this; var po = this.po;
            if (g.frame)
            {
                $(g.frame.document).ready(function ()
                {
                    if (g.windowMask) g.windowMask.hide();
                    g.dialog.hide();
                });
            }
            else
            {
                if (g.windowMask) g.windowMask.hide();
                g.dialog.hide();
            }
        },
        show: function ()
        {
            var g = this, p = this.options;
            if (p.modal)
                g._applyWindowMask();
            g.dialog.show();
        },
        setUrl: function (url)
        {
            var g = this, p = this.options;
            p.url = url;
            if (p.load)
            {
                g.dialog.body.html("").load(p.url, function ()
                {
                    g.trigger('loaded');
                });
            }
            else if (g.jiframe)
            {
                g.jiframe.attr("src", p.url);
            }
        },
        _applyWindowMask: function ()
        {
            var g = this; var po = this.po;
            if (g.windowMask) g.windowMask.remove();
            g.windowMask = $("<div class='l-window-mask' style='display: block;'></div>");
            g.windowMask.height($(window).height() + $(window).scrollTop()).appendTo('body');
        },
        _removeWindowMask: function ()
        {
            var g = this; var po = this.po;
            if (g.windowMask) g.windowMask.remove();
        },
        _applyDrag: function ()
        {
            var g = this; var po = this.po;
            if ($.fn.ligerDrag)
                g.dialog.ligerDrag({ handler: '.l-dialog-title', animate: false });
        },
        _applyResize: function ()
        {
            var g = this; var po = this.po;
            if ($.fn.ligerResizable)
            {
                g.dialog.ligerResizable({
                    onStopResize: function (current, e)
                    {
                        var top = 0;
                        var left = 0;
                        if (!isNaN(parseInt(g.dialog.css('top'))))
                            top = parseInt(g.dialog.css('top'));
                        if (!isNaN(parseInt(g.dialog.css('left'))))
                            left = parseInt(g.dialog.css('left'));
                        if (current.diffTop != undefined)
                        {
                            g.dialog.css({
                                top: top + current.diffTop,
                                left: left + current.diffLeft
                            });
                            g.dialog.body.css({
                                width: current.newWidth - 26
                            });
                            $(".l-dialog-content", g.dialog.body).height(current.newHeight - 46 - $(".l-dialog-buttons", g.dialog).height());
                        }
                        return false;
                    } , animate: false
                });
            }
        },
        _setImage: function ()
        {
            var g = this; var po = this.po;
            if (p.type)
            {
                if (p.type == 'success' || p.type == 'donne' || p.type == 'ok')
                {
                    $(".l-dialog-image", g.dialog).addClass("l-dialog-image-donne").show();
                    $(".l-dialog-content", g.dialog).css({ paddingLeft: 64, paddingBottom: 30 });
                }
                else if (p.type == 'error')
                {
                    $(".l-dialog-image", g.dialog).addClass("l-dialog-image-error").show();
                    $(".l-dialog-content", g.dialog).css({ paddingLeft: 64, paddingBottom: 30 });
                }
                else if (p.type == 'warn')
                {
                    $(".l-dialog-image", g.dialog).addClass("l-dialog-image-warn").show();
                    $(".l-dialog-content", g.dialog).css({ paddingLeft: 64, paddingBottom: 30 });
                }
                else if (p.type == 'question')
                {
                    $(".l-dialog-image", g.dialog).addClass("l-dialog-image-question").show();
                    $(".l-dialog-content", g.dialog).css({ paddingLeft: 64, paddingBottom: 40 });
                }
            }
        }
    });
    $.ligerui.controls.Dialog.prototype.hide = $.ligerui.controls.Dialog.prototype.hidden;



    $.ligerDialog.open = function (p)
    {
        return $.ligerDialog(p);
    };
    $.ligerDialog.close = function ()
    {
        $(".l-dialog,.l-window-mask").remove();
    };
    $.ligerDialog.show = function (p)
    {
        if ($(".l-dialog").length > 0)
        {
            $(".l-dialog,.l-window-mask").show();
            return;
        }
        return $.ligerDialog(p);
    };
    $.ligerDialog.hide = function ()
    {
        $(".l-dialog,.l-window-mask").hide();
    };
    $.ligerDialog.alert = function (content, title, type, callback)
    {
        content = content || "";
        if (typeof (title) == "function")
        {

            callback = title;
            type = null;
        }
        else if (typeof (type) == "function")
        {
            callback = type;
        }
        var btnclick = function (item, Dialog, index)
        {
            Dialog.close();
            if (callback)
                callback(item, Dialog, index);
        };
        p = {
            content: content,
            buttons: [{ text: '确定', onclick: btnclick}]
        };
        if (typeof (title) == "string" && title != "") p.title = title;
        if (typeof (type) == "string" && type != "") p.type = type;
        return $.ligerDialog(p);
    };

    $.ligerDialog.confirm = function (content, title, callback)
    {
        if (typeof (title) == "function")
        {
            callback = title;
            type = null;
        }
        var btnclick = function (item, Dialog)
        {
            Dialog.close();
            if (callback)
            {
                callback(item.type == 'ok');
            }
        };
        p = {
            type: 'question',
            content: content,
            buttons: [{ text: '是', onclick: btnclick, type: 'ok' }, { text: '否', onclick: btnclick, type: 'no'}]
        };
        if (typeof (title) == "string" && title != "") p.title = title;
        return $.ligerDialog(p);
    };
    $.ligerDialog.warning = function (content, title, callback)
    {
        if (typeof (title) == "function")
        {
            callback = title;
            type = null;
        }
        var btnclick = function (item, Dialog)
        {
            Dialog.close();
            if (callback)
            {
                callback(item.type);
            }
        };
        p = {
            type: 'question',
            content: content,
            buttons: [{ text: '是', onclick: btnclick, type: 'yes' }, { text: '否', onclick: btnclick, type: 'no' }, { text: '取消', onclick: btnclick, type: 'cancel'}]
        };
        if (typeof (title) == "string" && title != "") p.title = title;
        return $.ligerDialog(p);
    };
    $.ligerDialog.waitting = function (title)
    {
        title = title || $.ligerDefaults.Dialog.waittingMessage;
        return $.ligerDialog.open({ cls: 'l-dialog-waittingdialog', type: 'none', content: '<div style="padding:4px">' + title + '</div>', allowClose: false });
    };
    $.ligerDialog.closeWaitting = function ()
    {
        $(".l-dialog-waittingdialog,.l-window-mask").remove();
    };
    $.ligerDialog.success = function (content, title, onBtnClick)
    {
        return $.ligerDialog.alert(content, title, 'success', onBtnClick);
    };
    $.ligerDialog.error = function (content, title, onBtnClick)
    {
        return $.ligerDialog.alert(content, title, 'error', onBtnClick);
    };
    $.ligerDialog.warn = function (content, title, onBtnClick)
    {
        return $.ligerDialog.alert(content, title, 'warn', onBtnClick);
    };
    $.ligerDialog.question = function (content, title)
    {
        return $.ligerDialog.alert(content, title, 'question');
    };


    $.ligerDialog.prompt = function (title, value, multi, callback)
    {
        var target = $('<input type="text" class="l-dialog-inputtext"/>');
        if (typeof (multi) == "function")
        {
            callback = multi;
        }
        if (typeof (value) == "function")
        {
            callback = value;
        }
        else if (typeof (value) == "boolean")
        {
            multi = value;
        }
        if (typeof (multi) == "boolean" && multi)
        {
            target = $('<textarea class="l-dialog-textarea"></textarea>');
        }
        if (typeof (value) == "string" || typeof (value) == "int")
        {
            target.val(value);
        }
        var btnclick = function (item, Dialog, index)
        {
            Dialog.close();
            if (callback)
            {
                callback(item.type == 'yes', target.val());
            }
        }
        p = {
            title: title,
            target: target,
            width: 320,
            buttons: [{ text: '确定', onclick: btnclick, type: 'yes' }, { text: '取消', onclick: btnclick, type: 'cancel'}]
        };
        return $.ligerDialog(p);
    };


})(jQuery);