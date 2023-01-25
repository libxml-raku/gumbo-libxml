use Gumbo::LibXML;
use LibXML::Document;

multi MAIN(Str :$html!) {
    my LibXML::Document $doc = Gumbo::LibXML.parse-html: $html;
    say $doc.Str;
}

multi MAIN(IO() :$file!) {
    my LibXML::Document $doc = Gumbo::LibXML.parse-html: $file;
    say $doc.Str;
}
