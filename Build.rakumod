#!/usr/bin/env raku
class Build {
    method build($dir) {
        unless "Makefile".IO.e {
            unless "configure".IO.e {
                shell "./autogen.sh";
                die "autogen failed"
                    unless "configure".IO.e;
            }
            my $prefix = $dir.IO.absolute;
            shell "./configure";
            die "configure failed"
                unless "Makefile".IO.e;
        }
        shell("make");
        True;
    }
}

# Build.pm can also be run standalone
sub MAIN(Str $working-directory = '.' ) {
    Build.new.build($working-directory);
}
