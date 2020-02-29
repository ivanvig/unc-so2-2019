\NeedsTeXFormat{LaTeX2e}[1995/12/01]
\ProvidesClass{unc_so2}


% \DeclareOption*{\PassOptionsToClass{\CurrentOption}{article}}
% \ProcessOptions\relax
\LoadClass[article,oneside,11pt]{memoir}


\RequirePackage[letterpaper,top=1in,bottom=1in,left=1in,right=1in,headsep=\dimexpr 0.25in-1em,marginparwidth=1.25in]{geometry}


\RequirePackage{etoolbox}
\RequirePackage{DejaVuSans}
%% Another possibility is
%% \usepackage{dejavu}
%% which loads the DejaVu Serif and DejaVu Sans Mono fonts as well
\renewcommand*\familydefault{\sfdefault} %% Only if the base font of the document is to be sans serif

\RequirePackage{graphicx}
\RequirePackage{xcolor}
\definecolor{lblue}{RGB}{3,155,229}
\definecolor{lred}{RGB}{230,26,23}

\setsecheadstyle{\LARGE\color{lblue}}
\setsubsecheadstyle{\Large\color{lred}}

% \renewcommand{\thesection}{\arabic{section}}


\maxtocdepth{subsection}
\setlength{\cftsubsectionindent}{5em}
\RequirePackage{letterspace} % not so heavy...

% \renewcommand\printtoctitle[1]{\section*{#1}}
% \renewcommand\aftertoctitle{}
\renewcommand{\contentsname}{Índice}
\renewcommand{\listfigurename}{Índice de figuras}
\renewcommand{\listtablename}{Índice de tablas}

\newcommand{\unilogo}{\gdef\@unilogo}
\newcommand{\defenddate}{\gdef\@defenddate}

\counterwithout{section}{chapter}

\newcommand{\nombrefecha}{
  \begingroup
% a \clearpage will close the group and restore the meaning
\let\clearpage\endgroup

% here you type the text you want
\begin{flushleft}
  \LARGE
  \@author \\
  \textcolor{gray}{\@defenddate}
\end{flushleft}
\vspace{4\baselineskip}
% Now issue \tableofcontents
}


\renewcommand{\maketitle}[1][]{%
 \begin{titlingpage}
   \begin{flushleft}
   \OnehalfSpacing
   \let\footnotesize\small \let\footnoterule\relax \setcounter{page}{1}
   \vspace*{3\baselineskip}
       \includegraphics[height=1in]{\@unilogo} \\
       {\color{lblue}\fontsize{36}{43}\selectfont Cátedra de Sistemas Operativos II\par}
     {\SingleSpacing\fontsize{48}{58}\selectfont\textbf\@title\par}
   \end{flushleft}
 \end{titlingpage}
 \pagenumbering{roman}
 \setcounter{page}{2}
}               %% End of \maketitle


% \RequirePackage{imakeidx}
% \makeindex[columns=1,options=-s simple_letters]
