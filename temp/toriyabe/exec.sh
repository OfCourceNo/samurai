echo 'けします'
rm res/*
echo 'けしました'
ls res/
echo 'check!'
make
./manager/gameManager 1 1 > res/res11
./manager/gameManager 1 2 > res/res12
./manager/gameManager 1 3 > res/res13
./manager/gameManager 1 4 > res/res14
./manager/gameManager 1 5 > res/res15
./manager/gameManager 2 2 > res/res22
./manager/gameManager 2 3 > res/res23
./manager/gameManager 2 4 > res/res24
./manager/gameManager 2 5 > res/res25
./manager/gameManager 3 3 > res/res33
./manager/gameManager 3 4 > res/res34
./manager/gameManager 3 5 > res/res35
./manager/gameManager 4 4 > res/res44
./manager/gameManager 4 5 > res/res45
./manager/gameManager 5 5 > res/res55
make clean
echo 'check the count of line in each result'
cat res/res11 | wc -l
cat res/res12 | wc -l
cat res/res13 | wc -l
cat res/res14 | wc -l
cat res/res15 | wc -l
cat res/res22 | wc -l
cat res/res23 | wc -l
cat res/res24 | wc -l
cat res/res25 | wc -l
cat res/res33 | wc -l
cat res/res34 | wc -l
cat res/res35 | wc -l
cat res/res44 | wc -l
cat res/res45 | wc -l
cat res/res55 | wc -l
