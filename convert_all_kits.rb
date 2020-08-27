#!/usr/bin/env ruby

require 'fileutils'

Dir.glob("*/Ableton*/**/**/*.adg").select { |k| k.include?('Kit') }.each do |kit|
  kit = File.expand_path(kit)
  sitala_kit = kit.sub('/Ableton Live/', '/Sitala/')
                  .sub(/\/Presets\/[\d\. ]*Kits\//, '/')
                  .sub(/.adg$/, '.sitala')
  sitala_dir = sitala_kit.sub(/\/[^\/]+\.sitala$/, '')
  FileUtils.mkdir_p(sitala_dir)
  system('SamplesFromMarsConverter', kit, sitala_kit)
end
