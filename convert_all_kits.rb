#!/usr/bin/env ruby

require 'fileutils'

Dir.glob("*/Ableton*/**/**/*.adg").select { |k| k.include?('Kit') }.sort.each do |kit|
  path_components = kit.sub(/\/Ableton( Live){0,1}\//, '/Sitala/')
                      .sub(/.adg$/, '.sitala')
                      .split(File::SEPARATOR)
  FileUtils.mkdir_p(path_components[0,3].join(File::SEPARATOR))
  kit = File.expand_path(kit)
  sitala_kit = File.expand_path(
    [*path_components[0,3], path_components.last].join(File::SEPARATOR))
  system('SamplesFromMarsConverter', kit, sitala_kit)
end
