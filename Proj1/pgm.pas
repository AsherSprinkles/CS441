Program Program1;
Const
    numscores = 4;
Type
    student = record   (** ** similar to _a struct in C ** **)
	    name : string; 
		score: array[1..numscores] of real;
		avg  : real;
		letterGrade: string;
	end;
Var
	name : String;
	r    : student;
	i: integer;
	sum : real;
	ascore : integer;
Begin
    Write('What is your name? ');
	Readln(name);
	Writeln ( 'Hello ', name, '! Prepare to learn _PASCAL!!' ) ;
	

	(* process a student record *)
	sum := 0;
	r.name:=name;
	for i := 1 to numscores do
	    begin
		write('Enter score ',i,': ');
		readln( ascore );
		if ascore < 0 then ascore := 0;
		if ascore > 100 then ascore:= 100;
		ascore := ascore - 1;
		sum := sum + ascore;
		r.score[i] := ascore;
		end;
	r.avg := (1.0*sum)/numscores;
	if r.avg >= 85 then
	   r.letterGrade := 'A'
	else if r.avg <= 60 then
	   r.letterGrade := 'F'
	else
	   r.letterGrade :='B';
	writeln('The average is ',r.avg:5:1,' and the grade is ',r.letterGrade);
	writeln;
	writeln('Enter anything to continue....');
	readln; (*system pause*)
End.
   
