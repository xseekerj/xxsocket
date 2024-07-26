# Default manifest, refer in 1k/1kiss.ps1

if ($IsWin) {
    $manifest['nasm'] = '2.16.03+'
} else {
    $manifest['nasm'] = '2.15.05+'
}
