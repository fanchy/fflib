<?php
/*
Plugin Name: Syntax Highlighter and Code Prettifier Plugin for WordPress
Plugin URI: http://www.lastengine.com/syntax-highlighter-wordpress-plugin
Description: Simple and probably one of the best syntax highlighters in the market. This plugin integrates Alex Gorbatchev's Syntax Highlighter Tool : <a href="http://alexgorbatchev.com/wiki/SyntaxHighlighter">http://alexgorbatchev.com/wiki/SyntaxHighlighter</a>. Full Support for : Bash/shell, C#, C++, CSS, Delphi, Diff, Groovy, JavaScript, Java, Perl, PHP, Plain Text, Python, Ruby, Scala, SQL, Visual Basic and XML. 
Version: 2.1.364
Author: Vijesh Mehta
Author URI: http://www.lastengine.com
*/

function highlighter_header() {
	$current_path = get_option('siteurl') .'/wp-content/plugins/' . basename(dirname(__FILE__)) .'/';
	?>
	<link type="text/css" rel="stylesheet" href="<?php echo $current_path; ?>styles/shCore.css" />
	<link type="text/css" rel="stylesheet" href="<?php echo $current_path; ?>styles/shThemeDefault.css" />
	<script type="text/javascript" src="<?php echo $current_path; ?>scripts/shCore.js"></script>
	<script type="text/javascript" src="<?php echo $current_path; ?>scripts/shBrushBash.js"></script>
	<script type="text/javascript" src="<?php echo $current_path; ?>scripts/shBrushCpp.js"></script>
	<script type="text/javascript" src="<?php echo $current_path; ?>scripts/shBrushCSharp.js"></script>
	<script type="text/javascript" src="<?php echo $current_path; ?>scripts/shBrushCss.js"></script>
	<script type="text/javascript" src="<?php echo $current_path; ?>scripts/shBrushDelphi.js"></script>
	<script type="text/javascript" src="<?php echo $current_path; ?>scripts/shBrushDiff.js"></script>
	<script type="text/javascript" src="<?php echo $current_path; ?>scripts/shBrushGroovy.js"></script>
	<script type="text/javascript" src="<?php echo $current_path; ?>scripts/shBrushJava.js"></script>
	<script type="text/javascript" src="<?php echo $current_path; ?>scripts/shBrushJScript.js"></script>
	<script type="text/javascript" src="<?php echo $current_path; ?>scripts/shBrushPerl.js"></script>
	<script type="text/javascript" src="<?php echo $current_path; ?>scripts/shBrushPhp.js"></script>
	<script type="text/javascript" src="<?php echo $current_path; ?>scripts/shBrushPlain.js"></script>
	<script type="text/javascript" src="<?php echo $current_path; ?>scripts/shBrushPython.js"></script>
	<script type="text/javascript" src="<?php echo $current_path; ?>scripts/shBrushRuby.js"></script>
	<script type="text/javascript" src="<?php echo $current_path; ?>scripts/shBrushScala.js"></script>
	<script type="text/javascript" src="<?php echo $current_path; ?>scripts/shBrushSql.js"></script>
	<script type="text/javascript" src="<?php echo $current_path; ?>scripts/shBrushVb.js"></script>
	<script type="text/javascript" src="<?php echo $current_path; ?>scripts/shBrushXml.js"></script>
	<script type="text/javascript">		
		SyntaxHighlighter.config.clipboardSwf = '<?php echo $current_path; ?>scripts/clipboard.swf';
		SyntaxHighlighter.all();
	</script>
	<?php
}

add_action('wp_head','highlighter_header');
?>
