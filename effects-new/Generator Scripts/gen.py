from jinja2 import Environment, FileSystemLoader
import datetime

def capitalizeFirstLetterOnly(string):
    return string[:1].upper() + string[1:]

# make C/C++ double literal value
def makeDouble(string):
    if len(string) == 0:
        return '0.0'
    str = ('%f' % float(string)).rstrip('0')
    if str[-1] == '.':
        return str + '0'
    else:
        return str
        
# make C/C++ float literal value with trailing "f"
def makeFloat(string):
    return makeDouble(string) + 'f'
    
env = Environment(
    loader = FileSystemLoader('templates'),
    trim_blocks = True,
    lstrip_blocks = True
)
env.filters['capitalizeFirstLetterOnly'] = capitalizeFirstLetterOnly
env.filters['makeDouble'] = makeDouble
env.filters['makeFloat'] = makeFloat

vars = {
    'author': 'Shane Dunne',
    'year': str(datetime.datetime.now().year),
    'projectName': 'Leslie',
    'params': list(),
}

def doFile(templateFileName):
    template = env.get_template(templateFileName)
    filename = vars['projectName'] + templateFileName
    with open(filename, 'w') as outfile:
        outfile.write(template.render(vars).encode('utf-8'))

# Just the project name is enough to generate the Processor files
doFile('Processor.h')
doFile('Processor.cpp')

# Read the parameter descriptions
for line in open('Parameters.csv'):
    items = line[:-1].split(',')
    if items[0] == vars['projectName']:
        param = dict()
        param['descriptiveName'] = items[1]
        param['baseName'] = items[2]
        param['labelText'] = items[3]
        param['enumCount'] = 0 if len(items[4])==0 else int(items[4])
        param['unitOfMeasure'] = items[5]
        param['minValue'] = items[6]
        param['maxValue'] = items[7]
        param['defaultValue'] = items[8]
        param['stepValue'] = items[9]
        param['workingVar'] = items[10]
        vars['params'].append(param)

doFile('Parameters.h')
doFile('Parameters.cpp')
doFile('Editor.h')
doFile('Editor.cpp')
