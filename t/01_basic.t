use v6;
use LibXML::Document;
use LibXML::Text;
use Gumbo::LibXML;
use Test;

my $html = q:to/END_HTML/;
<html>
<head>
	<title>Fancy</title>
</head>
<body>
	<p>It's fancy</p>
</body>
</html>

END_HTML

plan 5;

my $xmldoc = Gumbo::LibXML.parse-html($html.encode);

ok $xmldoc ~~ LibXML::Document, "Return a XML::Document";
ok $xmldoc.root.name eq "html", "Root element is html";
ok $xmldoc.root.elements[0].elements[0][0] ~~ LibXML::Text, "Found text";
is $xmldoc<//title>[0].text, "Fancy", "Title is Fancy";

$xmldoc = Gumbo::LibXML.parse-html($html.encode, :nowhitespace);

is $xmldoc<//title>[0].name, "title";


