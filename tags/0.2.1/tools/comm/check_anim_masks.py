import sys
import os
import re


filename_pattern = re.compile(r'.+\.c$')

start_pattern = re.compile(r'.+AlienAmbientArray \(ambient animations\).+')
end_pattern = re.compile(r'(\t|    )},')

anim_pattern = re.compile(r'(\t|    )(\t|    |  ){\s*(//|/\*)\s*([0-9]+)?\ -?\s*(.+)')
anim_pattern2 = re.compile(r'^(\t|    )(\t|    ){$')
anim_end_pattern = re.compile(r'^(\t|    )(\t|    )},?$')

mask_line_pattern = re.compile('\s*(.+)/\* BlockMask \*/')
mask_line_pattern2 = re.compile('\s*[^(]*(\(1L?\s*<<\s*[0-9]+L?.*\))[^)]*$')
mask_part_regex = re.compile('\s*\(1L?\s*<<\s*([0-9]+)L?\s*\)\s*')


syntax_errors = 0
name_errors = 0
number_errors = 0
mask_errors = 0


def check_map(filename, map, count):
    global mask_errors
    
    def warn(s):
        print '%s: %s!' % (filename, s)
    
    if len(map) != count:
        warn('')
    for num, parts in map.items():
        for p in parts:
            if p not in map or num not in map[p]:
                warn('%d is blocked by %d but not vice-versa' % (num, p))
                mask_errors += 1


def check_file(filename):
    global syntax_errors, name_errors, number_errors, mask_errors

    print 'Opening: %s' % filename
    f = open(filename, 'rt')
    lines = f.read().split('\n')
    f.close()
    print 'Number of lines: %d' % len(lines)
    
    inside = False
    for i in range(len(lines)):
        line = lines[i].strip('\r\n')
        def warn(s):
            print '%s:%d: %s!' % (filename, i+1, s)
        
        if start_pattern.match(line):
            print 'Found start'
            inside = True
            anim_num = 0
            in_pattern = False
            blocked_by_map = {}
        
        if inside:
            if anim_pattern.match(line):
                if in_pattern:
                    warn('already inside pattern')
                    syntax_errors += 1
                m = anim_pattern.match(line)
                if m.groups()[3] is None:
                    comment_num = -1
                else:
                    comment_num = int(m.groups()[3])
                anim_name = m.groups()[4]
                if anim_name.endswith('*/'):
                    anim_name = anim_name[:-2]
                print 'Found anim: %d - %s' % (comment_num, anim_name)
                if comment_num != anim_num:
                    warn('anim num is actually %d' % anim_num)
                    number_errors += 1
                in_pattern = True
            
            if anim_pattern2.match(line) and not in_pattern:
                comment_num = -1
                anim_name = '?'
                print 'Found anim: %d - %s' % (comment_num, anim_name)
                name_errors += 1
                if comment_num != anim_num:
                    warn('anim num is actually %d' % anim_num)
                in_pattern = True
            
            if mask_line_pattern.match(line) or mask_line_pattern2.match(line):
                if not in_pattern:
                    warn("Mask outside pattern: '%s'" % line)
                    syntax_errors += 1
                m = mask_line_pattern.match(line)
                if m is None:
                    m = mask_line_pattern2.match(line)
                masks = m.groups()[0].rstrip(', ')
                #print 'Masks: %s' % masks
                parts = []
                for p in masks.split('|'):
                    m = mask_part_regex.match(p)
                    if not m:
                        continue
                    parts.append(int(m.groups()[0]))
                #print 'Parts: %s' % parts
                if anim_num in blocked_by_map:
                    blocked_by_map[anim_num].extend(parts)
                else:
                    blocked_by_map[anim_num] = parts
            
            if anim_end_pattern.match(line):
                if not in_pattern:
                    warn("pattern ends before starting: '%s'" % line)
                    syntax_errors += 1
                #print 'End of anim'
                anim_num += 1
                in_pattern = False
            
            if end_pattern.match(line):
                if in_pattern:
                    warn('end of pattern not found')
                    syntax_errors += 1
                print 'Found end'
                inside = False
                check_map(filename, blocked_by_map, anim_num)
    
    if inside:
        warn('end not found')
        syntax_errors += 1
    
    print 'Finished: %s' % filename


def main():
    for path, dirs, files in os.walk('.'):
        for fn in files:
            if filename_pattern.match(fn):
                filename = os.path.join(path, fn)
                check_file(filename)
    print syntax_errors, name_errors, number_errors, mask_errors


if __name__ == '__main__':
    main()
