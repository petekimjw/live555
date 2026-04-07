# live555 프로젝트 빌드방법
## 소스 : melchi45/live555 
https://github.com/melchi45/live555/blob/master/CMakeLists.txt?utm_source=chatgpt.com

## 다운로드 & 빌드 절차
1. 리포지터리 클론 (예: melchi45/live555 사용) : git clone https://github.com/melchi45/live555.git
1. cd live555 (혹은 wention/live555로 대체 가능)
2. 빌드 디렉터리 생성
- mkdir build
- cd build
3. CMake로 Visual Studio 2022 솔루션 생성 (cmake폴더에 생성됨)
- cmake .. -G "Visual Studio 17 2022" -A x64 -DLIVE555_ENABLE_OPENSSL=ON -DLIVE555_BUILD_EXAMPLES=ON
- 옵션 설명:
- -DLIVE555_ENABLE_OPENSSL=OFF: OpenSSL 사용 여부
- -DLIVE555_BUILD_EXAMPLES=ON: 테스트 프로그램 빌드 여부 
https://github.com/wention/live555/blob/master/CMakeLists.txt?utm_source=chatgpt.com

- 서버 : testOnDemandRTSPServer
- 클라이언트 : testRTSPClient

# Copilot 빌드
- VS Code 코파일럿으로 "CMakeLists.txt" 빌드해줘
- 빌드 성공 후, 솔루션 탐색기에서 "testOnDemandRTSPServer"와 "testRTSPClient" 프로젝트를 찾아 빌드
- testOnDemandRTSPServer : 
	- 출력디렉토리 :..\..\cmake\testProgs\Debug\
	- TLS 설정 : TLS 사용, 인증서 파일과 개인 키 파일 경로 지정
#define PATHNAME_TO_CERTIFICATE_FILE "e:\\sgkim\\Dev_OpenSource\\live555-master-melchi45-2\\live555-master\\build\\testProgs\\Debug\\localhost-100y.cert"
#define PATHNAME_TO_PRIVATE_KEY_FILE "e:\\sgkim\\Dev_OpenSource\\live555-master-melchi45-2\\live555-master\\build\\testProgs\\Debug\\localhost-100y.key"

- testRTSPClient
	- 출력디렉토리 :..\..\cmake\testProgs\Debug\
	- 디버깅/명령인수 : rtsps://user:!Q2w3e4r5t@172.16.15.121:322/ch1/stream1/media.imp?metadata=vixwin


# 테스트한 프로젝트 목록

- live555-master-melchi45 : cmake로 sln 생성(cmake폴더). 주석추가 + SHA-256 지원 
- live555-master-melchi45-2 : cmake로 sln 생성(build폴더)
- live555-master-melchi45-3 : cmake로 sln 생성(build폴더)
- live555-master-melchi45-4 : cmake로 sln 생성(build폴더)

# 프로젝트 파일에서 절대경로 -> 상대경로로 변경

```fix_path.ps1
$projectRoot = "E:\sgkim\Dev_OpenSource\live555-master-melchi45-2\live555-master"
# Both forward-slash and backslash variants of the paths
$absPathFwd = "E:/sgkim/Dev_OpenSource/live555-master-melchi45-2/live555-master"
$absPathBs  = "E:\sgkim\Dev_OpenSource\live555-master-melchi45-2\live555-master"
$absVcpkgFwd = "E:/sgkim/Dev_OpenSource/vcpkg"
$absVcpkgBs  = "E:\sgkim\Dev_OpenSource\vcpkg"

$files = Get-ChildItem -Path $projectRoot -Recurse -Filter "*.vcxproj" | Where-Object {
    (Get-Content $_.FullName -Raw) -match "[EeDd]:[/\\]sgkim"
}

Write-Host "Found $($files.Count) files to process"

$count = 0
foreach ($file in $files) {
    $content = Get-Content $file.FullName -Raw
    
    $relDir = $file.DirectoryName.Substring($projectRoot.Length).TrimStart('\', '/')
    if ($relDir -eq "") { $depth = 0 } else { $depth = ($relDir -split '[\\/]').Count }
    
    # Forward-slash relative paths
    if ($depth -eq 0) { $relToRootFwd = "." } else { $relToRootFwd = (@("..") * $depth) -join "/" }
    $relToVcpkgFwd = "$relToRootFwd/../../vcpkg"
    
    # Backslash relative paths
    if ($depth -eq 0) { $relToRootBs = "." } else { $relToRootBs = (@("..") * $depth) -join "\" }
    $relToVcpkgBs = "$relToRootBs\..\..\vcpkg"

    $newContent = $content
    
    # Replace forward-slash variants (vcpkg first - longer/more specific)
    $newContent = $newContent -replace [regex]::Escape($absVcpkgFwd), $relToVcpkgFwd
    $newContent = $newContent -replace [regex]::Escape($absPathFwd), $relToRootFwd
    
    # Replace backslash variants (vcpkg first - longer/more specific)
    $newContent = $newContent -replace [regex]::Escape($absVcpkgBs), $relToVcpkgBs
    $newContent = $newContent -replace [regex]::Escape($absPathBs), $relToRootBs
    
    if ($newContent -ne $content) {
        Set-Content -Path $file.FullName -Value $newContent -NoNewline -Encoding UTF8
        $count++
        Write-Host "Updated: $($file.FullName.Substring($projectRoot.Length)) (depth=$depth)"
    }
}

Write-Host "`nTotal files updated: $count"

# Verify
$remaining = Get-ChildItem -Path $projectRoot -Recurse -Filter "*.vcxproj" | Select-String -Pattern "[EeDd]:[/\\]sgkim" -List | Measure-Object
Write-Host "Remaining files with absolute paths: $($remaining.Count)"
```