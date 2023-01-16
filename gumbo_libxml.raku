use NativeCall;
use LibXML::Document;
use LibXML::Raw;
sub gumbo_libxml_parse(Blob --> xmlDoc) is native('gumbo_xml') {*}
sub gumbo_libxml_parse_with_options(Pointer, Blob, size_t --> xmlDoc) is native('libgumbo_xml') {*}

multi MAIN(:$file!) {
    my $string = $file.IO.slurp(:bin);
    MAIN :$string;
}

multi MAIN(:$string!) {
    my xmlDoc $raw = gumbo_libxml_parse($string);
    my LibXML::Document $doc .= new: :$raw;
    say $doc.Str;
}
