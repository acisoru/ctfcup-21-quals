a = [[79,55],[79,103,91],[79,103,91,85,82],[79,103,115,121,124,122],[79,55,43],[79,103,115,109,112],[79,55],[79,103,91,85],[79,103,91,97,100],[79,103,91,97],[79,55,43],[79,103,91,97,100],[79,103,115,109,106,107],[79,103,91,97,100],[79,55],[79,103,91,85],[79,103,91,97,100],[79,103,115,121],[79,103,91,97,100],[79,103,115,109,112,113],[79,55,43,49,52],[79,55],[79,103,91,85,82,83],[79,55,43,49],[79,55,43,49,52],[79,103,115,109,106],[79,103,115,121,124]]

template = '''cor = malloc(sizeof(struct correct_path));\ncor->vals = malloc(sizeof(int) * {});\ncor->size = {};\n'''
entry = '''cor->vals[{}] = {};\n'''
end = '''CorrectList[{}] = cor;\n'''

code = ''

for i in range(len(a)):
    elem = a[i]
    code += template.format(len(elem), len(elem))

    for j in range(len(elem)):
        code += entry.format(j, elem[j])
    
    code += end.format(i)

print(code)