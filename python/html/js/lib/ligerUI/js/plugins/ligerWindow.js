/**
* jQuery ligerUI 1.1.5
* 
* Author leoxie [ gd_star@163.com ] 
* 
*/
(function ($)
{

    $.ligerui.windowCount = 0;

    $.ligerWindow = function (options)
    {
        return $.ligerui.run.call(null, "ligerWindow", arguments, { isStatic: true });
    };

    $.ligerWindow.switchWindow = function (windowManager)
    {
        windowManager.window.css("z-index", "101").siblings(".l-window").css("z-index", "100");
    };
    $.ligerWindow.show = function (p)
    {
        return $.ligerWindow(p);
    };

    $.ligerDefaults.Window = {
        showClose: true,
        showMax: true,
        showToggle: true,
        title: 'window',
        load: false,
        onLoaded: null
    };

    $.ligerMethos.Window = {};

    $.ligerui.controls.Window = function (options)
    {
        $.ligerui.controls.Window.base.constructor.call(this, null, options);
    };
    $.ligerui.controls.Window.ligerExtend($.ligerui.core.UIComponent, {
        __getType: function ()
        {
            return 'Window';
        },
        __idPrev: function ()
        {
            return 'Window';
        },
        _extendMethods: function ()
        {
            return $.ligerMethos.Window;
        },
        _render: function ()
        {
            var g = this, p = this.options;
            g.window = $('<div class="l-window"><div class="l-window-header"><div class="l-window-header-buttons"><div class="l-window-toggle"></div><div class="l-window-max"></div><div class="l-window-close"></div><div class="l-clear"></div></div><div class="l-window-header-inner"></div></div><div class="l-window-content"></div></div>');
            g.element = g.window[0];
            g.window.content = $(".l-window-content", g.window);
            g.window.header = $(".l-window-header", g.window);
            if (p.url)
            {
                if (p.load)
                {
                    g.window.content.load(p.url, function ()
                    {
                        g.trigger('loaded');
                    });
                    g.window.content.addClass("l-window-content-scroll");
                }
                else
                {
                    var iframe = $("<iframe frameborder='0' src='" + p.url + "'></iframe>");
                    var framename = "ligeruiwindow" + $.ligerui.windowCount++;
                    if (p.name) framename = p.name;
                    iframe.attr("name", framename).attr("id", framename);
                    p.framename = framename;
                    iframe.appendTo(g.window.content);
                    g.iframe = iframe;
                }
            }
            else if (p.content)
            {
                var content = $("<div>" + p.content + "</div>");
                content.appendTo(g.window.content);
            }
            else if (p.target)
            {
                p.target.appendTo(g.window.content);
            }
            g.active();

            $('body').append(g.window);
            //设置参数属性  
            p.width && g.window.width(p.width);
            p.height && g.window.content.height(p.height - 28);

            //位置初始化
            var left = 0;
            var top = 0;
            if (p.left != null) left = p.left;
            else left = 0.5 * ($(window).width() - g.window.width());
            if (p.top != null) top = p.top;
            else top = 0.5 * ($(window).height() - g.window.height()) + $(window).scrollTop() - 10;
            if (left < 0) left = 0;
            if (top < 0) top = 0;
            g.window.css({ left: left, top: top });

            p.title && $(".l-window-header-inner", g.window.header).html(p.title);
            p.framename && $(">iframe", g.window.content).attr('name', p.framename);
            if (!p.showToggle) $(".l-window-toggle", g.window).remove();
            if (!p.showMax) $(".l-window-max", g.window).remove();
            if (!p.showClose) $(".l-window-close", g.window).remove();

            //拖动支持
            if ($.fn.ligerDrag)
            {
                g.window.drag = g.window.ligerDrag({ handler: '.l-window-header-inner', onStartDrag: function ()
                {
                    g.active();
                    //g.window.addClass("l-window-dragging"); 
                }, onStopDrag: function ()
                {
                    //g.window.removeClass("l-window-dragging"); 
                }, animate: false
                });
            }
            //改变大小支持
            if ($.fn.ligerResizable)
            {
                g.window.resizable = g.window.ligerResizable({
                    onStartResize: function ()
                    {
                        g.active();
                        $(".l-window-max", g.window).removeClass("l-window-regain");
                    },
                    onStopResize: function (current, e)
                    {
                        var top = 0;
                        var left = 0;
                        if (!isNaN(parseInt(g.window.css('top'))))
                            top = parseInt(g.window.css('top'));
                        if (!isNaN(parseInt(g.window.css('left'))))
                            left = parseInt(g.window.css('left'));
                        if (current.diffTop)
                            g.window.css({ top: top + current.diffTop });
                        if (current.diffLeft)
                            g.window.css({ left: left + current.diffLeft });
                        if (current.newWidth)
                            g.window.width(current.newWidth);
                        if (current.newHeight)
                            g.window.content.height(current.newHeight - 28);
                        return false;
                    }
                });
                g.window.append("<div class='l-btn-nw-drop'></div>");
            }
            //设置事件 
            $(".l-window-toggle", g.window).click(function ()
            {
                if ($(this).hasClass("l-window-toggle-close"))
                {
                    $(this).removeClass("l-window-toggle-close");
                } else
                {
                    $(this).addClass("l-window-toggle-close");
                }
                g.window.content.slideToggle();
            }).hover(function ()
            {
                if (g.window.drag)
                    g.window.drag.set('disabled', true);
            }, function ()
            {
                if (g.window.drag)
                    g.window.drag.set('disabled', false);
            });
            $(".l-window-close", g.window).click(function ()
            {
                if (g.trigger('close') == false) return false;
                g.window.hide();
            }).hover(function ()
            {
                if (g.window.drag)
                    g.window.drag.set('disabled', true);
            }, function ()
            { 
                if (g.window.drag)
                    g.window.drag.set('disabled', false);
            });
            $(".l-window-max", g.window).click(function ()
            {
                if ($(this).hasClass("l-window-regain"))
                {
                    if (g.trigger('regain') == false) return false;
                    g.window.width(g.lastWindowWidth).css({ left: g.lastWindowLeft, top: g.lastWindowTop });
                    g.window.content.height(g.lastWindowHeight - 28);
                    $(this).removeClass("l-window-regain");
                }
                else
                {
                    if (g.trigger('max') == false) return false;
                    g.lastWindowWidth = g.window.width();
                    g.lastWindowHeight = g.window.height();
                    g.lastWindowLeft = g.window.css('left');
                    g.lastWindowTop = g.window.css('top');
                    g.window.width($(window).width() - 2).css({ left: 0, top: 0 });
                    g.window.content.height($(window).height() - 28);
                    $(this).addClass("l-window-regain");
                }
            });

            g.set(p);
        },
        hide: function ()
        {
            this.window.hide();
        },
        show: function ()
        {
            this.window.show();
        },
        remove: function ()
        {
            this.window.remove();
        },
        //设置为活动状态(置于最前端)
        active: function ()
        {
            $.ligerWindow.switchWindow(this);
        },
        setUrl: function (url)
        {
            var g = this, p = this.options;
            p.url = url;
            if (p.load)
            {
                g.window.content.html("").load(p.url, function ()
                {
                    if (g.trigger('loaded') == false) return false;
                });
            }
            else if (g.jiframe)
            {
                g.jiframe.attr("src", p.url);
            }
        }
    });

})(jQuery);