require 'rmagick'

class ResizedImage
  attr_reader :magick, :mime_type

  def initialize result
    image_blob = result.gsub(/.*Content-type.+\s+/, '')
    @mime_type = result.scan(/.*Content-type:\s*(.+)\r/).flatten.first

    init_magick(image_blob)
    dump_to_file(image_blob, mime_type)
  end

  private

  def init_magick image_blob
    @magick = Magick::Image.from_blob(image_blob).first
  end

  def dump_to_file image_blob, mime_type
    test_image_file = "test_image.#{mime_type.split('/').pop}"
    File.unlink(test_image_file) if File.exists?(test_image_file)
    File.open(test_image_file, 'wb').write(image_blob)
  end
end