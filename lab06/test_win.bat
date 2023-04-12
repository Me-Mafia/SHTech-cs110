COPY .\ex1.circ .\testing\circ_files
COPY .\ex2.circ .\testing\circ_files
COPY .\ex3.circ .\testing\circ_files
CD testing
DEL /q student_output
MD student_output
python ./test.py
cd ..
pause
