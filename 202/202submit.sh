# if user gave an arg, pass it along to autocomment+
if [ -z $1 ]
then
	python3 ~/.bin/acp.py
else
	python3 ~/.bin/acp.py $1
fi

# if user gave an arg, pass it along to headercheck
if [ -z $1 ]
then
	python3 ~/.bin/202headercheck.py
else
	python3 ~/.bin/202headercheck.py $1 
fi
