#!/usr/bin/python

import sys
import os
import zipfile
import Image
import ImageFile
import ImageStat
import StringIO
import re


DEFAULT_COMPRESSION_LEVEL = 90
DEFAULT_SIZE_THRESHOLD = 1024
DEFAULT_REDUCTION_THRESHOLD = 0.1
DEFAULT_ALPHA_THRESHOLD = 128
DEFAULT_SIZE_WARNING_THRESHOLD = 100*1024

DEFAULT_FORCE_REGEX = r'.+/cutscene/.+\.png'


def notice(msg):
    print >>sys.stderr, msg

def warning(msg):
    print >>sys.stderr, msg

class Processor(object):
    def __init__(self):
        self.compression_level = DEFAULT_COMPRESSION_LEVEL
        self.size_threshold = DEFAULT_SIZE_THRESHOLD
        self.reduction_threshold = DEFAULT_REDUCTION_THRESHOLD
        self.alpha_threshold = DEFAULT_ALPHA_THRESHOLD
        self.size_warning_threshold = DEFAULT_SIZE_WARNING_THRESHOLD
        
        self.force_regex = re.compile(DEFAULT_FORCE_REGEX)

        self.ani_fixes = {}
        self.image_replacements = {}
        self.images_seen = {}
        self.savings = 0
        
        self.total_pixels = 0

    def has_transparency(self, im):
        if im.mode == 'RGB':
            return False

        if im.mode == 'RGBA' and ImageStat.Stat(im).extrema[3][0] >= self.alpha_threshold:
            return False

        return True

    def analyse_png(self, png_name):
        f = self.get_data(png_name)
        buf = f.read()
        f.close()

        p = ImageFile.Parser()
        p.feed(buf)
        im = p.close()
        
        self.total_pixels += im.size[0] * im.size[1]
        
        if len(buf) < self.size_threshold:
            #notice('File %s does not meet size threshold (size is %d)' % (png_name, len(buf)))
            return False

        if self.force_regex.match(png_name):
            warning('File %s will be forced' % png_name)
        elif self.has_transparency(im):
            #notice('File %s has transparent pixels' % png_name)
            if len(buf) >= self.size_warning_threshold:
                warning('Transparent image is quite large: %s (%d bytes)' % (png_name, len(buf)))
            return False

        ss = StringIO.StringIO();
        try:
            im.save(ss, 'JPEG', quality=self.compression_level)
        except IOError, ex:
            warning('Unable to convert %s to JPEG: %s' % (png_name, ex))
            return False
        buf2 = ss.getvalue()
        reduction = (len(buf) - len(buf2))/float(len(buf))
        if reduction < self.reduction_threshold:
            #notice('File %s does not meet reduction threshold (reduction was %d%%)' % (png_name, 100*reduction))
            return False

        self.savings += len(buf) - len(buf2)
        self.image_replacements[png_name] = buf2;
        return True

    def analyze_ani(self, ani_name):
        f = self.get_data(ani_name)
        changed = False
        new_lines = []
        line_num = 0
        for line in f:
            line = line.strip()
            line_num += 1
            if line == '':
                warning('Blank line %s:%d' % (ani_name, line_num))
                new_lines.append(line)
                continue
            parts = line.split()
            if len(parts) != 5:
                warning('Malformed ANI line %s:%d: %s' % (ani_name, line_num, line))
            if parts[0].endswith('.png'):
                png_name = os.path.join(os.path.dirname(ani_name), parts[0]).replace('\\', '/')
                
                if png_name in self.images_seen:
                    self.images_seen[png_name] += 1
                else:
                    self.images_seen[png_name] = 1
                
                try:
                    if self.analyse_png(png_name):
                        changed = True
                        parts[0] = parts[0].replace('.png', '.jpg')
                        new_lines.append(' '.join(parts))
                        warning('File %s referenced by %s:%d is compressable!' % (png_name, ani_name, line_num))
                    else:
                        new_lines.append(line)
                except KeyError:
                    warning('File %s referenced by %s:%d cannot be analysed!' % (png_name, ani_name, line_num))
                    new_lines.append(line)
                    continue
            else:
                new_lines.append(line)

        f.close()
        if line_num != len(new_lines):
            warning('New ani content is different length from old!')
        if changed:
            self.ani_fixes[ani_name] = '\n'.join(new_lines)

    def analyze(self):
        ani_names = [name for name in self.names if name.endswith('.ani')]
        for ani_name in ani_names:
            self.analyze_ani(ani_name)
        notice('Compressable images: %d' % len(self.image_replacements))
        notice('Expected savings: %d' % self.savings)
        notice('Total number of pixels: %d' % self.total_pixels)
    
    def apply(self):
        self.create()
        for name in self.names:
            if (name.endswith('.png') or name.endswith('.jpg')) and name not in self.images_seen:
                warning('Image %s not seen in any ANI file!' % name)
            if name in self.image_replacements:
                self.write_data(name.replace('.png', '.jpg'), self.image_replacements[name])
            elif name in self.ani_fixes:
                self.write_data(name, self.ani_fixes[name])
            else:
                f = self.get_data(name)
                self.write_data(name, f.read())
                f.close()


class UQMProcessor(Processor):
    def __init__(self, uqm_name):
        super(UQMProcessor, self).__init__()
        self.uqm_name = uqm_name
        self.input = zipfile.ZipFile(uqm_name, 'r')
        self.names = self.input.namelist()
        self.output = None

    def get_data(self, name):
        f = self.input.open(name, 'r')
        return f

    def write_data(self, name, data):
        self.output.writestr(name, data)

    def close(self):
        self.input.close()
        if self.output is not None:
            self.output.close()
            #os.rename(self.output_name, self.uqm_name)
    
    def create(self):
        self.output_name = self.uqm_name.replace('.uqm', '.compress.uqm')
        self.output = zipfile.ZipFile(self.output_name, 'w')


def main(args=None):
    if args is None:
        args = sys.argv

    uqm_name = args[1]

    p = UQMProcessor(uqm_name)
    p.analyze()
    p.apply()
    p.close()


if __name__ == '__main__':
    exit(main())
