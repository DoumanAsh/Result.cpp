$ErrorActionPreference= 'silentlycontinue'
$REPO=git config remote.origin.url

rm -re -fo html/ -ErrorAction SilentlyContinue
git clone $REPO html
cd html/
git checkout gh-pages
if ($LASTEXITCODE) {
    git checkout --orphan gh-pages
}
git rm -rf .

cd ../

$SHA=git log -1 --format="%s(%h %cd)" --date=short

doxygen | Write-Host

cd html/

git status

if (git status --porcelain) {
    git add .
    git commit -m "Auto-update" -m "Commit: ${SHA}"
    git push origin HEAD

    cd ../

    rm -re -fo html/
}
else {
    echo "No changes to docs"
}
