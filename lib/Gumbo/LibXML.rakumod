unit class Gumbo::LibXML;

use NativeCall;
use LibXML::Document;
use LibXML::Raw;

our constant $GUMBO-LIBXML is export = %?RESOURCES<libraries/gumbo_xml>;

our sub gumbo_libxml_parse(Blob() --> xmlDoc) is native($GUMBO-LIBXML) {*}
our sub gumbo_libxml_parse_with_options(Pointer, Blob, size_t --> xmlDoc) is native($GUMBO-LIBXML) {*}

multi method parse-html(Str:D $html, |c) is hidden-from-backtrace {
    self.parse-html: $html.encode, |c;
}

multi method parse-html(Blob:D $html, |c) is hidden-from-backtrace {
    my xmlDoc $raw = gumbo_libxml_parse($html);
    LibXML::Document.new: :$raw;
}

multi method parse-html(IO::Path:D $file, |c) is hidden-from-backtrace {
    self.parse-html: $file.open, |c;
}

multi method parse-html(IO::Handle:D $fh, |c) is hidden-from-backtrace {
    self.parse-html: $fh.slurp(:bin), |c;
}

multi method parse-html(Failure $_, |c) is hidden-from-backtrace {
    .throw;
}
