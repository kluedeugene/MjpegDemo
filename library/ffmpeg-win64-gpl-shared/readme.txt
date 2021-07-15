폴더의 내용들은 FFMPEG 라이브러리의 구성요소들이다.
opencv와 혼용해서 인코딩하므로 

(디버깅 탭)
	     디버깅 환경을  PATH=C:\opencv\build\x64\vc15\bin;%PATH%
(VC++ 디렉터리 탭)
프로젝트의 포함 디렉터리를 C:\opencv\build\include;$(IncludePath)
       라이브러리 디렉터리를 C:\opencv\build\x64\vc15\lib;$(LibraryPath) 추가.
(C/C++ 탭)
         추가 포함 디렉터리를 C:\opencv\build\include

위와같이 설정된 상태에서 각각의 bin, include , lib 폴더에 맞는 파일들을 복사한다.
bin 파일은 용량이 커서 첨부되지않았음.ㅣ
