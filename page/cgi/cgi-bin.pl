#!/usr/bin/perl
use strict;
use warnings;
use CGI;

my $cgi = CGI->new;
my $filehandle = $cgi->upload('fileToUpload');

if (!$filehandle) {
    print $cgi->header(-status => '400 Bad Request');
    print "파일을 전송하지 않았습니다.";
    exit;
}

my $filename = $cgi->param('fileToUpload');
my $upload_dir = '/path/to/upload/directory';  # 업로드할 디렉토리 경로 설정

# 파일 저장
open(my $upload_file, '>', "$upload_dir/$filename") or die "파일 저장 실패: $!";
binmode $upload_file;
while (my $chunk = <$filehandle>) {
    print $upload_file $chunk;
}
close $upload_file;

print $cgi->header;
print "파일 업로드 완료: $filename";
