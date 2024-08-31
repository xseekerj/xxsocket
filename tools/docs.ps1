# gen docs, requires python3.x and install dependencies:
#  pip install -r docs/requirements.tx
# usage: ./scripts/docs.ps1 '3.39.11:v3.39.11,3.39.12:3.39.x'
$rel_str = $args[0]

mkdocs --version
mike --version
mike delete --all

Write-Host "Active versions: $rel_str"

$docs_ver = $null
if ($rel_str) {
    $rel_arr = ($rel_str -split ',')
    foreach($rel in $rel_arr) {
        # ver:tag
        $info = ($rel -split ':')
        $docs_ver = $info[0]
        $docs_tag = if ($info.Count -ge 2) { $info[1] } else { "v$($info[0])" }
        git checkout "$docs_tag"
        mike deploy $docs_ver
    }
}

git checkout dev
mike deploy latest
mike list

if ($docs_ver -and ($env:GITHUB_ACTIONS -eq 'true')) {
    mike set-default $docs_ver --push
}
