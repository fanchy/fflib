
// JavaScript Document

//finish by Lecaf
//2011.3.23
/*添加事件*/
var addEventHandler=function(oTarget, sEventType, fnHandler){
	romoveEventHandler(oTarget, sEventType, fnHandler);
	if (oTarget.addEventListener){
		oTarget.addEventListener(sEventType, fnHandler, false);
	}else if(oTarget.attachEvent){
		oTarget.attachEvent("on" + sEventType, fnHandler);
	}else{
		oTarget["on" + sEventType] = fnHandler;
	}
}
/*注销事件*/
var romoveEventHandler=function(oTarget, sEventType, fnHandler){
	if (oTarget.removeEventListener){
		oTarget.removeEventListener(sEventType, fnHandler, false);
	}else if(oTarget.detachEvent){
		oTarget.detachEvent("on" + sEventType, fnHandler);
	}else{
		oTarget["on" + sEventType] = "";
	}
}
function editor(){
	var ieRange=false;
	var edit = document.createElement("iframe");
	edit.style.width = "500px";
	edit.style.height = "300px";
	edit.frameBorder=1;
	document.getElementsByTagName("body")[0].appendChild(edit);
	var win=edit.contentWindow;
	var doc=win.document;
	var _saveRange=function(){
		//IE下保存Range对象
		if(!!document.all&&!ieRange){//是否IE并且判断是否保存过Range对象
			var sel=doc.selection;
			ieRange=sel.createRange();
			if(sel.type!='Control'){//选择的不是对象
				var p=ieRange.parentElement();//判断是否在编辑器内
				if(p.tagName=="INPUT"||p==document.body)ieRange=false;
			}        
		}    
	}    
	var _insert=function(text){//插入替换字符串
	if (!!ieRange){
		ieRange.pasteHTML(text);
			ieRange.select();
			ieRange = false; //清空下range对象
		}else{//焦点不在html编辑器内容时
			win.focus();
			if(document.all){
				doc.body.innerHTML += text; //IE插入在最后
			}else{//Firefox
				var sel = win.getSelection();
				var rng = sel.getRangeAt(0);
				var frg = rng.createContextualFragment(text);
				rng.insertNode(frg); 
			}
		}    
	}
	var _ieEnter=function(){//IE回车修改
		var e = win.event;
		if(e.keyCode == 13){
			_saveRange();
			_insert("<br/>");
			return false;
		}
	}
	var _fnPaste=function(e){
		e.returnValue = false;
		var shtml=window.clipboardData.getData("Text"); //安全起见,只取纯文本
		_saveRange();
		_insert(shtml);//把文本粘贴进iframe
	}
	if(document.all){
		addEventHandler(edit,"load",function(){//绑定编辑器粘帖事件onpaste;
			with(doc.getElementsByTagName("body")[0]){
				addEventHandler(doc.getElementsByTagName("body")[0],"paste",function(event){
					_fnPaste(event);
　　　　　　	});
			}
		});
	}
	doc.designMode='On';//可编辑
	doc.contentEditable = true;
	//但是IE与FireFox有点不同，为了兼容FireFox，所以必须创建一个新的document。
	doc.open();
	var headHTML='<head><meta http-equiv="Content-Type" content="text/html; charset=utf-8" />';
　　	var headHTML= headHTML + '<style>*{margin:0;padding:0;font:12px;}</style>';
　　	var headHTML= headHTML +'</head>';
	doc.writeln('<html>'+headHTML+'<body></body></html>');
	doc.close();
	//IE回车输出<br> 与 FF 统一；
	if(document.all)doc.onkeypress = function(){return _ieEnter()};
	win.focus();
}
window.onload=editor;
