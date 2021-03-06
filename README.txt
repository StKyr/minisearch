Systems Programming - Project 1

Kyriakos Stylianopoulos - 1115201400194


----------------------------------------------------------
ΔΙΑΤΑΞΗ ΑΡΧΕΙΩΝ ΠΑΡΑΔΟΤΕΟΥ:

+--scr/                 : Περιέχει τα source code files.
     + main.c
     + commandParsing.c
     + index.c
     + postingList.c
     + search.c
     + scoring.c
     + printer.c
+--include/             : Περιέχει τα header files.
     + commandParsing.h
     + index.h
     + postingList.h
     + search.h
     + scoring.h
     + printer.h
+--bin/                 : Θα περιέχει τα objective files κατά την μεταγλώττιση.
+--makefile             : Το αρχείο για την εντολή make.
+--README.txt           : Το παρόν αρχείο.

------------------------------------------------------------
ΜΕΤΑΓΛΩΤΤΙΣΗ:

$ make
ή
$ make clean && make

--------------------------------------------------------------
ΕΚΤΕΛΕΣΗ:

$ ./minisearch -k K -i inputDoc.txt

--------------------------------------------------------------

ΠΕΡΙΓΡΑΦΗ ΚΩΔΙΚΑ ΚΙ ΕΠΙΛΟΓΕΣ ΣΧΕΔΙΑΣΗΣ:

  Το αρχείο main.c ειναι αφενός υπέυθυνο για το parsing των ορισματων γραμμών εντολής και αφετέρου επαναληπτικά διαβάζει την επόμενη εντολή από τον χρήστη και καλεί την αντίστοιχη διαδικασία του search.c, συνεχίζοντας μέχρι να δοθεί η εντολή /exit ή κάποια μη έγκυρη εντολή.
Το parsing των ορισμάτων γίνεται με την χρήση της getopt του GNU και τα αποτελέσματα σώζονται στην global δομή Params. Ευθύνη της main είναι επίσης και η κλήση της διαδικασίας για την ανάλυση και αποθήκευση των κειμένων εισόδου και για την δημιουργία του ευρετηρίου.
  Η ανάλυση των εντολών του χρήστη γίνεται στο αρχείο commandPrasing.c. Χρησιμοποιείται η δομή δεδομένων Command την οποία αποθηκεύται ο τύπος της εντολής και τα ορίσματά της. Σύμφωνα με αυτά η main μπορεί εύκολα να καλέσει την κατάλληλη διαδικασία.
  Στο αρχείο index.c βρίσκεται η υλοποίηση της δομής Index. Αυτή η δομή ενθυλακώνει τα Map, Trie(Inverted index) σε μία και μόνο δομή έτσι ώστε να δώσει ένα απλούστερο interface στον client code (main.c search.c). Οι συναρτήσεις που υπάρχουν εκεί, πέρα από την δημιουργία και καταστροφή της δομής, είναι απλά wrappers των συναρτήσεων των δύο εσωτερικών δομών.
  Στο ίδιο αρχείο βρίσκεται και η υλοποίηση της Map. Η Map περιέχει έναν πίνακα όσο το πλήθος των κειμένων. Σε κάθε του γραμμή, αντί να σώζονται απλά τα κείμενα ως text, σώζονται δεδομένα τύπου struct Document τα οποία είναι στην ουσία ένας δείκτης στο text κάθε κειμένου και ο αριθμός των λέξεων του. Η επιλογή αυτή έγινε καθώς η πληροφορία αυτή (του πλήθους των λέξεων) είναι αναγκαία για τον υπολογισμό του σκορ κάθε κειμένου. Και καθώς διατρέχονται αυτές οι λέξεις μία προς μία κατά την εισαγωγή τους στην Trie θεωρήθηκε σκόπιμο να αποθηκευτεί αυτή η πληροφορία αντί να αναδημιουργείται κάθε φορά. Τέλος, η Map περιέχει και ως πεδία το averagedl και το size (αριθμός κειμένων) του ευρετηρίου.
  Η υλοποίηση της Trie βρίσκεται στο αρχείο Trie.c. Η ίδια η δομή είναι αρκετά απλή (περιέχει μόνο έναν δείκτη στον κόμβο-ρίζα της και το μέγιστο πλήθος λέξης, δηλ. το ύψος της). Στην ουσία η λειτουργική δομή είναι η TrieNode (έχει εμβέλεια μόνο μέσα στο Trie.c). Όλες οι συναρτήσεις της Trie στην ουσία καλούν τις αναδρομικές συναρτήσεις της TrieNode με αρχικό όρισμα την ρίζα. Η δομή αυτή αποθηκεύει τα αδέρφια σε αλφαβητική σειρά, έτσι ώστε να εκτυπωθουν οι λέξεις αλφαβητικά με ευκολία.
Άξίζει να αναφερθούν οι λεπτομέρειες για το πώς διατρέχεται η δομή (κατα την εντολή /df): Όπως ειπώθηκε η Trie αποθηκεύει το μέγιστο βάθος του δέντρου. Η τιμή αυτή χρησιμοποιείται ώστε να οριστεί στην αρχή της αναδρομής ένας πίνακας χαρακτήρων αυτού του μεγέθους οποίος λειτουργεί ως στοίβα. Δηλαδή κάθε κόμβος εισάγει στην τελευταία θέση τον χαρακτήρα που έχει αποθηκεύσει. Όταν φτάσουμε σε τερματικό κόμβο έχει στην ουσία σχηματιστεί ολήκληρη η λέξη στην stack (αν την διατρέξουμε από το πρώτο της στοιχείο), οπότε και μπορούμε να την εκτυπώσουμε. Το γεγονός ότι ο πίνακας είναι μεγέθους ίσου του μέγιστου βάθους σημαίνει ότι ακομα και η μεγαλύτερη λέξη μπορεί να αποθηκευτεί χωρίς πρόβλημα και χωρίς να χρειάζεται να γίνουν αναρίθμητες malloc και free. Μάλιστα, το μέγεθος αυτό δεν είναι σπατάλη μνήμης καθώς αργά ή γρήγορα θα αναγκαστούμε να αποθηκεύσουμε την μεγαλύτερη λέξη και άρα θα δεσμεύσουμε τόση μνήμη.
Ένα άλλο σημείο που πρέπει να επισημανθεί είναι πως η συνάρτηση αυτή δεν εκτυπώνει η ίδια καμμία λέξη ή document frequency. Κάτι τέτοιο θα χαλούσε τελείως την απόκρυψη των δεδομένων καθώς θα έπρεπε μια αποθηκευτική δομή να γνωρίζει τόσο για το πώς λειτουργεί η posting list όσο και για το τι είναι τα frequencies των λέξεων και πώς πρέπει να εκτυπωθούν. Για να λυθεί το παραπάνω πρόβλημα, η συνάρτηση που κάνει traverse δέχεται ως όρισμα ένα δείκτη σε συνάρτηση ώς callback η οποία και δέχεται ορίσματα char * και PostingList *. H Traverse από την μεριά της καλεί την callback κάθε φορά που σχηματίζεται μια λέξη και φτάνει σε τερματικό κόμβο, περνώντας της την λέξη και την συσχετιζόμενη posting list. Έτσι η callback συνάρτηση μπορεί να εκτυπώσει.
  Τα σκορ των κειμένων κατά την εντολή /search υπολογίζονται με την χρήση του αρχείου scoring.c. Ο υπολογισμός γίνεται με βάση τον τύπο BM25, με τις παραμέτρους που δόθηκαν. Ως συχνότητα των λέξεων στα κείμενα χρησιμοποιείται το raw count τους.
  Οι διαδικασίες που κινούνται από την main και αποτελούν την «λογική» του ευρετηρίου βρίσκονται στο search.c. Οι συναρτήσεις για την εκτύπωση των αποτελεσμάτων των εντολών /tf και /df είναι αρκετά απλές στην υλοποίησή τους.
Η συνάρτηση για την εύρεση των αποτελεσμάτων της εντολής /search αποτελεί τον πυρήνα του προγράμματος. Έννοιολογικά, θεωρούμε την δομή RelevanDocument (του scoring.h) η οποία περίεχει τα πεδία documentId, score και έχει σκοπό να αποθηκεύει τα κείμενα τα οποία περιέχουν τουλάχιστον έναν από τους όρους αναζήτησης, ώστε να μπορεί εύκολα να γίνει κατάταξη κι εκτύπωση μεταξύ τους. Η συνάρτηση αυτή σε πρώτο πλάισιο βρίσκει όλα αυτά τα κέιμενα (ID's). Τα πρώτα K από αυτά εισάγονται σε έναν πίνακα (αφου υπολογιστούν τα σκορ τους) και ταξινομόυνται κατά φθίνουσες τιμές σκορ. Για κάθε ένα από τα υπόλοιπα relevant documents, υπολγίζεται το σκορ τους και μετά αναζητάται η θέση στον πίνακα στην οπόια θα έπρεπε, βάσει τα σκορ, να βρίσκεται αυτό το κείμενο. Αν όντως υπάρχει τέτοια θέση, τότε όλα τα "από κάτω" κέιμενα μετακινούνται μία θέση παρακάτω (με το τελευταίο να "πετάται") και το νέο κείμενο εισάγεται στην σωστή του θέση. Η διαδικασία αυτή, για κάθε νέο κείμενο παίρνει O(K) και η αρχική ταξινόμηση κοστίζει O(KlogK). Η διαδικασία αυτή δεν είναι κοστοβόρα όυτε από άποψη μνήμης (αποθηκεύονται μόνο K κείμενα) ούτε από άποψη χρόνου, καθώς ακόμα και με πιο γρήγορη δομή όπως μια heap, πέρα από την εισαγωγή νέου κειμένου που γίνεται σε O(logK), η τελική ταξινόμηση για την εκτύπωση (στην ουσία η εκτέλεση της heapsort) θα χρειαστεί χρόνο O(KlogK), οπότε η τελική ασυμπτωτική πολυπλοκότητα της διαδικασίας είναι η ίδια. Σε γενικές γραμμές λοιπόν, η υλοποίηση με ταξινομημένο πίνακα μεγέθους K δεν αποτελεί σπατάλη.
  Η διαδικασίες εκτύπωσης βρίσκονται σε ξεχωριστό αρχείο καθώς χρειάζονται αρκετές συναρτήσεις για την υλοποίησή τους. Για την εκτύπωση του κάθε κειμένου της εντολής search ακολουθείται η εξής διαδικασία: Αρχικά το κείμενο αντιγράφεται σε ένα άλλο έτσι ώστε οι χρακτήρες TAB να έχουν αντικατασταθεί με του αντίστοιχους κενούς χαρακτήρες (υποθέτοντας ότι το tabsize είναι 8), έτσι ώστε να εκτυπωθούν τα αντίστοιχα κενά μέχρι το επόμενο tabstop (ή το τέλος γραμμής). Για αυτή τη διαδικασία δεσμεύεται αρχικά χώρος 8πλάσιος του αρχικού κειμένου καθώς είναι το άνω φράγμα του μεγέθους του νέου κειμένου. Αφού αντιγραφεί το νέο string ο περιττός χώρος αποδεσμεύεται. Εδώ θα ήθελα να επισημάνω πως αν και αυτό μοιάζει με σπατάλη, εντούτοις δεν είναι τόσο μεγάλη όσο φαίνεται. Αυτό γιατί η διαδικασία αυτή γίνεται για κάθε κείμενο αλλα μετά αποδεσμέυεται. Άρα η μνήμη δεν αυξάνεται με την αύξηση του μεγέθους του προγράμματος. Για την ακρίβεια, είναι ακριβώς σαν να σώζονται στην μνήμη 8 παραπάνω κείμενα, δηλαδή ένα ελάχιστο overhead. Το να μην κατέληγα σε αυτή τη λύση θα οδηγούσε αναπόφευκτα σε λιγότερο ευανάγνωστο κώδικα.
Κατά την εκτύπωση, εκτυπώνονται αρχικά (στην πρώτη γραμμή), στοιχισμένα, οι πληροφορίες για το κάθε κείμενο όπως ζητήθηκαν. Για τις υπόλοιπες γραμμές εκτυπώνονται τα άντίστοιχα κενά. Για κάθε γραμμή κειμένου εκτυπώνονται οι χαρακτήρες έως το τέλος της γραμμής. Έπειτα καλείται μια συνάρτηση που δημιουργεί το string της υπογράμμισης (με κενα και '^') για την συγκεκριμένη γραμμη. Η διαδικασία αυτή αναγκαστικά πρέπει πρώτα να υπολγίσει το συγκεκριμένο string γιατί πρέπει να ελεγχθεί μήπως δεν υπάρχει κανένα '^' στην γραμμή όπότε και δεν πρέπει να εκτυπωθεί.
  Η PostingList βρίσκεται στο postingList.c και έχει υλοποιηθεί με βάση τις οδηγίες της εκφώνησης με την μόνη διαφορά πως ο τπος δεδομένων που την αναπαριστά περιέχει και ως πληροφορία το πλήθος των κόμβων της, το οποίο είναι ο αριθμός των κειμένων στα οποία εμφανίζεται η αντίστοιχη λέξη.

Μία σχηματική απεικόνηση των δομών, των module και των σχέσεων τους διατυπώνεται στο παρακάτω σχεδιάγραμμα:


          +----------+               +---------------+
   +------|  main    |-------------->|commandParsing |
   |      | -module- |               |  -module-     |
   |      +----------+               +---------------+
   |           |   |                                |
   |           |   |            +----------+        |
   |           |   +----------->| Command  |<-------+
   |           |                | -struct- |
   |           |                +----------+
   |           |
   |           |
   |           |
   |           |                                   +----------+
   |           |           +-----------+           | printer  |
   |           +---------->| search    |---------->| -module- |
   |                       | -module-  |           +----------+
   |                       +-----------+
   |                             |  |
   |   +------------+            |  |
   +-->|  Index     |<-----------+  |   +---------+
  +----|  -struct-  |               +-->| scoring |---+
  |    +------------+                   | -module-|   |
  |                |                    +---------+   |
  |                |                                  |
  |                |                                  |
  |                |                                  |
  | +----------+   |     +------------+               |
  +>| Map      |   |     |  Trie      |               |
    | -struct- |-+ +---->|  -struct-  |               |
    +----------+ |       +------------+               |
                 |            |                       |
                 |            |                       |
                 |            |      +------------+   |
   +----------+  |            +----->|PostingList |<--+
   | Document |<-+                   | -struct-   |
   | -struct- |                      +------------+
   +----------+

